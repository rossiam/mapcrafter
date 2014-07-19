/*
 * Copyright 2012-2014 Moritz Hilscher
 *
 * This file is part of Mapcrafter.
 *
 * Mapcrafter is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Mapcrafter is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Mapcrafter.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CONCURRENTQUEUE_H_
#define CONCURRENTQUEUE_H_

#include "../../util.h"

#include <queue>

#ifdef OPT_USE_BOOST_THREAD
#  include <boost/thread.hpp>
#  define THREAD_NS boost
#else
#  include <condition_variable>
#  include <mutex>
#  define THREAD_NS std
#endif

namespace mapcrafter {
namespace thread {

template<typename T>
class ConcurrentQueue {
public:
	ConcurrentQueue();
	~ConcurrentQueue();

	bool empty();
	void push(T item);
	T pop();

private:
	std::queue<T> queue;
	THREAD_NS::mutex mutex;
	THREAD_NS::condition_variable condition_variable;
};

template<typename T>
ConcurrentQueue<T>::ConcurrentQueue() {
}

template<typename T>
ConcurrentQueue<T>::~ConcurrentQueue() {
}

template<typename T>
bool ConcurrentQueue<T>::empty() {
	THREAD_NS::unique_lock<THREAD_NS::mutex> lock(mutex);
	return queue.empty();
}

template<typename T>
void ConcurrentQueue<T>::push(T item) {
	THREAD_NS::unique_lock<THREAD_NS::mutex> lock(mutex);
	if (queue.empty()) {
		queue.push(item);
		condition_variable.notify_one();
	} else {
		queue.push(item);
	}
}

template<typename T>
T ConcurrentQueue<T>::pop() {
	THREAD_NS::unique_lock<THREAD_NS::mutex> lock(mutex);
	while (queue.empty())
		condition_variable.wait(lock);
	T item = queue.front();
	queue.pop();
	return item;
}

} /* namespace thread */
} /* namespace mapcrafter */

#endif /* CONCURRENTQUEUE_H_ */