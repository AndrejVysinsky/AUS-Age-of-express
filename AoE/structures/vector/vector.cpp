#include "vector.h"
#include "../ds_routines.h"
#include <cstdlib>
#include <cstring>

namespace structures {

	Vector::Vector(size_t size) :
		memory_(calloc(size, 1)),
		size_(size)
	{
	}

	Vector::Vector(const Vector& other) :
		Vector(other.size_)
	{
		memcpy(this->memory_, other.memory_, other.size_);
	}

	Vector::Vector(Vector&& other) :
		memory_(other.memory_),
		size_(other.size_)
	{
		other.memory_ = nullptr;
		other.size_ = 0;
	}

	Vector::~Vector()
	{
		free(this->memory_);
		this->memory_ = nullptr;
		this->size_ = 0;
	}

	Structure* Vector::clone() const
	{
		return new Vector(*this);
	}

	size_t Vector::size() const
	{
		return size_;
	}

	Structure & Vector::operator=(const Structure & other)
	{
		if (this != &other)
		{
			*this = dynamic_cast<const Vector&>(other);
		}
		return *this;
	}

	Vector& Vector::operator=(const Vector& other)
	{
		if (this != &other) {
			this->size_ = other.size_;
			this->memory_ = realloc(this->memory_, this->size_);			
			memcpy(this->memory_, other.memory_, this->size_);
		}
		return *this;
	}

	Vector & Vector::operator=(Vector && other)
	{
		if (this != &other) {
			free(memory_);
			memory_ = other.memory_;
			size_ = other.size_;
			other.memory_ = nullptr;
			other.size_ = 0;
		}
		return *this;
	}

	bool Vector::operator==(const Vector& other) const
	{
		return this->size_ == other.size_ && memcmp(this->memory_, other.memory_, this->size_) == 0;
	}

	byte& Vector::operator[](const int index)
	{
		DSRoutines::rangeCheckExcept(index, this->size_, "mimo pole");
		return *(reinterpret_cast<byte*>(memory_) + index);
	}

	byte Vector::operator[](const int index) const
	{
		DSRoutines::rangeCheckExcept(index, this->size_, "mimo pole");
		return *(reinterpret_cast<byte*>(memory_) + index);
	}

	byte& Vector::readBytes(const int index, const int count, byte& dest)
	{
		DSRoutines::rangeCheckExcept(index, this->size_, "mimo pole");
		DSRoutines::rangeCheckExcept(index + count, this->size_, "mimo pole");
		memcpy(&dest, getBytePointer(index), count);
		return dest;
	}

	void Vector::copy(const Vector& src, const int srcStartIndex, Vector& dest, const int destStartIndex, const int length)
	{
		/*
			int a = 5; 
			if (a) -> a != 0
			if (!a) -> a == 0
		*/
		if (length) {
			DSRoutines::rangeCheckExcept(srcStartIndex, src.size_, "mimo pole");
			DSRoutines::rangeCheckExcept(destStartIndex, dest.size_, "mimo pole");
			DSRoutines::rangeCheckExcept(srcStartIndex + length, src.size_ + 1, "mimo pole");
			DSRoutines::rangeCheckExcept(destStartIndex + length, dest.size_ + 1, "mimo pole");

			if (src.memory_ == dest.memory_ && (srcStartIndex + length <= destStartIndex || destStartIndex + length <= srcStartIndex)) {
				memmove(src.getBytePointer(destStartIndex), src.getBytePointer(srcStartIndex), length);
			} else {
				memcpy(dest.getBytePointer(destStartIndex), src.getBytePointer(srcStartIndex), length);
			}
		}
	}

	byte* Vector::getBytePointer(const int index) const
	{
		DSRoutines::rangeCheckExcept(index, this->size_, "mimo pole");
		return reinterpret_cast<byte*>(this->memory_) + index;
	}

}