/*
 * CircleBuffer
 *
 *  Created on: 02.06.2023
 *      Author: Shpagin_D
 */

#ifndef CIRCLEBUFFER_H
#define CIRCLEBUFFER_H

#include <cstring> // ��� memset

template <class T, int L>
class CircleBuffer
{
private:
	int _capacity { L };	//< �������
	int _begin { 0 };		//< ������� �������
	int _end { 0 };			//< ������� ����������
	int _size { 0 };		//< ������� ��������� ���������
	T _buffer[L];			//< �����


public:
	/// ���������� ���������� ����������� ��������� ������
	int Size() const { return _size; }
	
	/// ���������� ���������� ���������� ��������� ������
	int Length() const { return _capacity; }
	
	/// ���������� true, ���� ����� ����������
	bool IsFull() const { return _size == _capacity; }
	
	/// ���������� true, ���� ����� ������
	bool IsEmpty() const { return _size == 0; }
	
	/// ���������� ������ �� ������� ������ �� �������. � ������, ���� ����� ������������, �� ������ �������� ������������� �� ������
	/// \Warning ��� �������� � ������� ������, � ������, ���� ����� ������, ����� �������� �������� ������
	T& GetValue(int index) { return _buffer[(_begin + index) % _capacity]; }
	
	/// ���������� ������ �� ������ ������� ������. � ������, ���� ����� ������������, �� ������ ������� �������� �������� ������������� �� ������
	/// \Warning ��� �������� � ������� ������, � ������, ���� ����� ������, ����� �������� �������� ������
	T& GetFrontValue() { return _buffer[_begin]; }
	
	/// ���������� ������ �� ��������� ������� ������. � ������, ���� ����� ������������, �� ������ ���������� �������� �������� ������������� �� ������
	/// \Warning ��� �������� � ������� ������, � ������, ���� ����� ������, ����� �������� �������� ������
	T& GetBackValue() { return _buffer[_end]; }

	/// ������������� �������� �������� �� �������. � ������, ���� ����� ������������, �� ������ �������� ������������� �� ������
	void SetValue(int index, T value) { GetValue(index) = value; }

	/// ���������� ����������� ������ �� ������� ������ �� �������. � ������, ���� ����� ������������, �� ������ �������� ������������� �� ������
	/// \Warning ��� �������� � ������� ������, � ������, ���� ����� ������, ����� �������� �������� ������
	const T& GetValue(int index) const { return _buffer[(_begin + index) % _capacity]; }

	/// ���������� ����������� ������ �� ������ ������� ������. � ������, ���� ����� ������������, �� ������ ������� �������� �������� ������������� �� ������
	/// \Warning ��� �������� � ������� ������, � ������, ���� ����� ������, ����� �������� �������� ������
	const T& GetFrontValue() const { return _buffer[_begin]; }

	/// ���������� ����������� ������ �� ��������� ������� ������. � ������, ���� ����� ������������, �� ������ ���������� �������� �������� ������������� �� ������
	/// \Warning ��� �������� � ������� ������, � ������, ���� ����� ������, ����� �������� �������� ������
	const T& GetBackValue() const { return _buffer[_end]; }

	/// ������� ��������� ������� � ������
	void EraseBack()
	{
		if (!IsEmpty())
		{
			_end = (_end - 1 + _capacity) % _capacity;
			--_size;
		}
	}

	/// ������� ������ ������� � ������
	void EraseFront()
	{
		if (!IsEmpty())
		{
			_begin = (_begin + 1) % _capacity;
			--_size;
		}
	}

	/// ��������� ������� � ����� ������
	void AddValueBack(T value)
	{
		if (!IsEmpty())
			_end = (_end + 1) % _capacity;
		_buffer[_end] = value;
		if (!IsFull())
			_begin = (_end - _size + _capacity) % _capacity;
		else 
			_begin = (_end + 1 + _capacity) % _capacity;
		_size = (_size + 1 > _capacity) ? _capacity : (_size + 1);
	}

	/// ��������� ������� � ������ ������
	void AddValueFront(T value)
	{
		if (!IsEmpty())
			_begin = (_begin - 1 + _capacity) % _capacity;
		_buffer[_begin] = value;
		_end = (_begin + _size) % _capacity;
		_size = (_size + 1 > _capacity) ? _capacity : (_size + 1);
	}

	/// ������� �����
	void Clear()
	{
		memset(_buffer, 0, _capacity * sizeof(T));
		_begin = 0;
		_end = 0;
		_size = 0;
	}
};

#endif // CIRCLEBUFFER_H
