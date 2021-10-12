#include "timer_wheel.h"
#include "logger.h"
#include <chrono>


TimerWheel::Wheel::Wheel(int level) :
	wheel_level_(level) {
	mask_ = WHEEL_0_MASK;
	if (wheel_level_ != 0) {
		nshift_ = WHEEL_0_BIT + WHEEL_N_BIT * (wheel_level_ - 1);
		mask_ = WHEEL_N_MASK;
	}
	nodes_.resize(mask_ + 1);
}

void TimerWheel::Wheel::purgeAllTo(NodesList* list) {
	for (auto& n : nodes_) {
		list->take(&n);
	}
	cur_idx_ = 0;
}

void TimerWheel::Wheel::reposit(NodesList* list) {
	auto node = list->pop();
	while (node) {
		int val = (node->node_val_ >> nshift_) & mask_;
		get((0 == wheel_level_ ? val : (val - 1)) & mask_)->push(node);
		node = list->pop();
	}
}


TimerWheel::TimerWheel(int wheel_level, int granularity_msec) :
	wheel_level_(std::max(wheel_level, WHEEL_LEVEL_MIN)),
	granularity_msec_(granularity_msec) {
	wheels_.push_back(new Wheel(0));
	for (int i = 1; i < wheel_level_; ++i) {
		wheels_.push_back(new Wheel(i));
	}
}

TimerWheel::~TimerWheel() {
	over_ = true;
	if (sp_thread_.get() && sp_thread_->joinable())
		sp_thread_->join();

	NodesList list;
	for (int i = 0; i < wheel_level_; ++i) {
		wheels_[i]->purgeAllTo(&list);
	}
	while (auto node = list.pop()) {
		node->destroy();
	}
}

void TimerWheel::start() {
	sp_thread_.reset(new std::thread(std::bind(&TimerWheel::run, this)));
}

void TimerWheel::exit() {
	over_ = true;
	if (sp_thread_.get() && sp_thread_->joinable())
		sp_thread_->join();
}

void TimerWheel::schedule(int duration_msec, NodeBase* node, const char* name) {
	duration_msec = (std::max)(0, duration_msec);
	node->node_duration_msec_ = duration_msec;
	int dur = duration_msec / granularity_msec_;
	NodeBase* to_del = nullptr;
	{
		std::lock_guard lock(mutex_);

		if (over_ || CANCELLING == node->node_state_) {
			to_del = node;
			node->node_state_ = NONE;
		} else if (READY == node->node_state_) {
			LOG(fatal, "node already scheduled!");
		} else {
			node->node_state_ = READY;
			int val = dur + wheels_value_;
			node->node_val_ = val;
			NodesList* curlist = wheels_[0]->get(val & WHEEL_0_MASK);
			for (int i = wheel_level_ - 1; i >= 1; --i) {
				Wheel* w = wheels_[i];
				int v = (val >> w->nshift_) & w->mask_;
				if (v != w->cur_idx_) {
					curlist = w->get((v - 1) & w->mask_);
					break;
				}
			}

			++nodes_count_;
			LOG(trace, "timerwheel[" << this << "] scheduled " << duration_msec << " ms, total=" << nodes_count_ << ", class=" << (name ? name : typeid(*node).name()));
			curlist->push(node);
		}
	}
	if (to_del)
		to_del->destroy();
}

bool TimerWheel::remove(NodeBase* node) {
	{
		std::lock_guard lock(mutex_);
		if (RUNNING == node->node_state_ || CANCELLING == node->node_state_) {
			node->node_state_ = CANCELLING;
			return false;
		}

		node->node_state_ = NONE;
		if (node->node_prev_)
			node->node_prev_->node_next_ = node->node_next_;
		if (node->node_next_)
			node->node_next_->node_prev_ = node->node_prev_;
		node->node_prev_ = node->node_next_ = nullptr;
		--nodes_count_;
	}
	node->destroy();
	return true;
}

size_t TimerWheel::getNodesCount() const {
	size_t total = 0;
	for (size_t i = 0; i < wheels_.size(); ++i) {
		total += wheels_[i]->nodes_.size();
	}
	return total;
}

int TimerWheel::step(int step_msec) {
	while (step_msec >= granularity_msec_) {
		step_msec -= granularity_msec_;
		NodesList active;
		{
			std::lock_guard lock(mutex_);
			if (over_)
				return -1;

			active.take(wheels_[0]->current());
			int i = 0;
			for (; i < wheel_level_ - 1; ++i) {
				if (0 != wheels_[i]->next_idx())
					break;
			}

			for (; i > 0; --i) {
				wheels_[i - 1]->reposit(wheels_[i]->current());
				wheels_[i]->step();
			}
			wheels_[0]->step();
			++wheels_value_;

			nodes_count_ -= active.setState(RUNNING);
		}

		while (auto node = active.pop()) {
			bool reschedule = false;
			try {
				node->onTimeout(&reschedule);
			} catch (std::exception& e) {
				LOG(warning, "timeout throw exception! " << e.what());
			}

			if (!reschedule)
				node->destroy();
			else
				schedule(node->node_duration_msec_, node);
		}
	}
	return step_msec;
}

void TimerWheel::run() {
	auto last_msec = std::chrono::steady_clock::now();
	for (;;) {
		auto cur = std::chrono::steady_clock::now();
		int left = step(static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(cur - last_msec).count()));
		if (left < 0)
			return;
		last_msec = cur - std::chrono::milliseconds(left);
		assert(left < granularity_msec_);
		std::this_thread::sleep_for(std::chrono::milliseconds((std::max)(100, granularity_msec_ - left)));
	}

}

