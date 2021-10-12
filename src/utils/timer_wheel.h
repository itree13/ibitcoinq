#ifndef _NDIOT_UTILS_TIMER_WHEEL_H_
#define _NDIOT_UTILS_TIMER_WHEEL_H_

#include "../type.h"
#include <functional>
#include <thread>
#include <mutex>
#include <vector>
#include <cassert>


#define WHEEL_0_BIT 		8
#define WHEEL_N_BIT 		6
#define WHEEL_0_SIZE 		(1 << WHEEL_0_BIT)
#define WHEEL_N_SIZE 		(1 << WHEEL_N_BIT)
#define WHEEL_0_MASK 		(WHEEL_0_SIZE - 1)
#define WHEEL_N_MASK 		(WHEEL_N_SIZE - 1)
#define WHEEL_LEVEL_MIN		2


class TimerWheel {
	DISABLE_COPY(TimerWheel);
public:
	enum NodeState { NONE, READY, RUNNING, CANCELLING };

	class NDIOT_DDS_API NodeBase {
		friend class Wheel;
		friend class TimerWheel;
		NodeBase* node_prev_ = nullptr;
		NodeBase* node_next_ = nullptr;
		NodeState node_state_ = NONE;
		int node_val_ = 0;
		int node_duration_msec_ = 0;

	protected:
		NodeBase() = default;
		virtual ~NodeBase() = default;

		virtual void destroy() {}
		virtual void onTimeout(bool* reschedule) {}
	};

	class Node : public NodeBase {
		DISABLE_COPY(Node);
		std::function<void(Node*)> deletor_;
	public:
		Node(std::function<void(Node*)> deletor = nullptr) : deletor_(deletor) {}

	private:
		virtual void destroy() {
			if (deletor_)
				deletor_(this);
			else
				delete this;
		}
	};

	class TimerId : public std::enable_shared_from_this<TimerId> {
		DISABLE_COPY(TimerId);

		friend class TimerWheel;

		class NodeF : public NodeBase {
		public:
			NodeF(std::shared_ptr<TimerId> timerid,
				std::function<void(bool*)> callback) :
				timerid_(timerid),
				callback_(callback) {
			}

		protected:
			virtual void destroy() {
				timerid_.reset();
			}

			virtual void onTimeout(bool* reschedule) {
				callback_(reschedule);
			}

		private:
			std::shared_ptr<TimerId> timerid_;
			std::function<void(bool*)> callback_;
		};

	public:
		TimerId() {
		}

		~TimerId() {
			delete nodef_;
		}

	private:
		NodeF* create(std::function<void(bool*)> callback) {
			return nodef_ = new NodeF(this->shared_from_this(), callback);
		}

	private:
		NodeF* nodef_ = nullptr;
	};

	typedef std::shared_ptr<TimerId> TimerIdPtr;

private:
	class NodesList {
		NodeBase top_;
		NodeBase tail_;
	public:
		NodesList() { top_.node_next_ = &tail_; tail_.node_prev_ = &top_; }

		void push(NodeBase* node) {
			assert(!node->node_prev_ && !node->node_next_);
			node->node_prev_ = tail_.node_prev_;
			tail_.node_prev_->node_next_ = node;
			tail_.node_prev_ = node;
			node->node_next_ = &tail_;
		}

		NodeBase* pop() {
			auto t = top_.node_next_;
			if (t != &tail_) {
				top_.node_next_ = t->node_next_;
				top_.node_next_->node_prev_ = &top_;
				t->node_prev_ = t->node_next_ = nullptr;
				return t;
			}
			return nullptr;
		}

		void take(NodesList* list) {
			if (list->top_.node_next_ != &list->tail_) {
				tail_.node_prev_->node_next_ = list->top_.node_next_;
				list->top_.node_next_->node_prev_ = tail_.node_prev_;
				list->tail_.node_prev_->node_next_ = &tail_;
				tail_.node_prev_ = list->tail_.node_prev_;
				list->top_.node_next_ = &list->tail_;
				list->tail_.node_prev_ = &list->top_;
			}
		}

		int setState(NodeState state) {
			int c = 0;
			for (auto p = top_.node_next_; p->node_next_; p = p->node_next_, ++c) {
				p->node_state_ = state;
			}
			return c;
		}

		int getCount() const {
			int c = 0;
			for (auto p = top_.node_next_->node_next_; p; p = p->node_next_, ++c);
			return c;
		}
	};

	class Wheel {
		friend class TimerWheel;
	public:
		Wheel(int level);

		NodesList* current() {
			return &nodes_[cur_idx_];
		}

		NodesList* get(size_t idx) {
			assert(idx < nodes_.size());
			return &nodes_[idx];
		}

		int idx() const {
			return cur_idx_;
		}

		int next_idx() {
			return (cur_idx_ + 1) % nodes_.size();
		}

		int step() {
			return cur_idx_ = next_idx();
		}

		void purgeAllTo(NodesList* list);
		void reposit(NodesList* list);

	private:
		const int wheel_level_;
		int mask_;
		int nshift_ = 0;
		std::vector<NodesList> nodes_;
		int cur_idx_ = 0;
	};

public:
	TimerWheel(int wheel_level = 4, int granularity_msec = 100);
	virtual ~TimerWheel();

	void start();
	void exit();

	size_t getNodesCount() const;

	int step(int step_msec);

	void schedule(int duration_msec, NodeBase* node, const char* name = nullptr);

	TimerIdPtr schedule(std::chrono::milliseconds duration_msec, std::function<void(bool*)> func, const char* name = nullptr) {
		TimerIdPtr timerid(new TimerId());
		schedule(static_cast<int>(duration_msec.count()), timerid->create(func), name);
		return timerid;
	}

	void remove(const TimerIdPtr& timerid) {
		if (timerid->nodef_)
			remove(timerid->nodef_);
	}

private:
	bool remove(NodeBase* node);

	void run();

private:
	const int wheel_level_;
	const int granularity_msec_;
	bool over_ = false;

	std::vector<Wheel*> wheels_;

	typedef std::mutex mutex_t;
	mutex_t mutex_;

	std::unique_ptr<std::thread> sp_thread_;

	int wheels_value_ = 0;

	int nodes_count_ = 0;
};


#endif /* BASE_TIMER_WHEEL_H_ */
