#ifndef _CONCURRENT_QUEUE_H_
#define _CONCURRENT_QUEUE_H_

#include "type.h"
#include <deque>
#include <mutex>
#include <condition_variable>

template <class T>
class ConcurrentQueueT {
public:
	ConcurrentQueueT() {
	}

	~ConcurrentQueueT() {
		abort();
	}

	void push(T&& v) {
		bool need_noti = false;
		{
			std::unique_lock lock(mutex_);
			if (!over_) {
				need_noti = q_.empty();
				q_.emplace_back(std::move(v));
			}
		}
		if (need_noti)
			available_cond_.notify_all();
	}

	bool pop(T* v) {
		std::unique_lock lock(mutex_);
		while (q_.empty()) {
			if (over_)
				return false;
			available_cond_.wait(lock);
		}
		if (v)
			*v = std::move(q_.front());
		q_.pop_front();
		return true;
	}

	bool pop(T* v, std::chrono::milliseconds milliseconds) {
		std::unique_lock lock(mutex_);
		while (q_.empty()) {
			if (over_)
				return false;
			if (std::cv_status::timeout == available_cond_.wait_for(lock, milliseconds)
				&& q_.empty())
				return false;
		}
		if (v)
			*v = std::move(q_.front());
		q_.pop_front();
		return true;
	}

	bool tryPop(T* v) {
		std::unique_lock lock(mutex_);
		if (q_.empty())
			return false;
		*v = std::move(q_.front());
		q_.pop_front();
		return true;
	}

	void abort() {
		std::unique_lock lock(mutex_);
		over_ = true;
		available_cond_.notify_all();
	}

	void clear() {
		std::unique_lock lock(mutex_);
		q_.clear();
	}

	bool isAborted() const {
		std::unique_lock lock(mutex_);
		return over_;
	}

	size_t size() const {
		return q_.size();
	}

private:
	bool over_ = false;
	mutable std::mutex mutex_;
	std::condition_variable available_cond_;
	std::deque<T> q_;
};


#endif
