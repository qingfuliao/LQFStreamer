#pragma once
#include <vector>
#include <memory>
#include <atomic>
#include <iostream>

template <typename T>
class LQFRingBuffer
{
public:
	LQFRingBuffer(unsigned capacity = 60)
		: buffer_(capacity)
		, capacity_(capacity)
		, num_datas_(0)
	{ }

	~LQFRingBuffer() {	}

	bool Push(const T& data) 
	{ 
		return pushData(std::forward<T>(data)); 
	}
	bool Push(T&& data) 
	{
		return pushData(data); 
	}

	bool Pop(T& data)
	{
		if (num_datas_ > 0)
		{
			data = std::move(buffer_[get_pos_]);
			add(get_pos_);
			num_datas_--;
			return true;
		}

		return false;
	}

	bool IsFull()  const { return ((num_datas_ == capacity_) ? true : false); }
	bool IsEmpty() const { return ((num_datas_ == 0) ? true : false); }
	int Size() const { return num_datas_; }

private:
	template <typename F>
	bool pushData(F&& data)
	{
		if (num_datas_ < capacity_)
		{
			buffer_[put_pos_] = std::forward<F>(data);
			add(put_pos_);
			num_datas_++;
			return true;
		}

		return false;
	}

	void add(int& pos)
	{
		pos = (((pos + 1) == capacity_) ? 0 : (pos + 1));
	}

	int capacity_ = 0;
	int put_pos_ = 0;
	int get_pos_ = 0;

	std::atomic_int num_datas_;
	std::vector<T> buffer_;
};
