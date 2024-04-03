/*
 * CircleBuffer
 *
 *  Created on: 02.06.2023
 *      Author: Shpagin_D
 */

#ifndef CIRCLEBUFFER_H
#define CIRCLEBUFFER_H

#include <cstring> // для memset

template <class T, int L>
class CircleBuffer
{
private:
	int _capacity { L };	//< емкость
	int _begin { 0 };		//< позиция первого
	int _end { 0 };			//< позиция последнего
	int _size { 0 };		//< сколько элементов заполнено
	T _buffer[L];			//< буфер


public:
	/// Возвращает количество заполненных элементов буфера
	int Size() const { return _size; }
	
	/// Возвращает количество выделенных элементов буфера
	int Length() const { return _capacity; }
	
	/// Возвращает true, если буфер переполнен
	bool IsFull() const { return _size == _capacity; }
	
	/// Возвращает true, если буфер пустой
	bool IsEmpty() const { return _size == 0; }
	
	/// Возвращает ссылку на элемент буфера по индексу. В случае, если буфер переполнялся, то индекс проходит переадресацию на буфере
	/// \Warning Нет привязки к размеру буфера, в случае, если буфер пустой, можно получить неверные данные
	T& GetValue(int index) { return _buffer[(_begin + index) % _capacity]; }
	
	/// Возвращает ссылку на первый элемент буфера. В случае, если буфер переполнялся, то индекс первого элемента проходит переадресацию на буфере
	/// \Warning Нет привязки к размеру буфера, в случае, если буфер пустой, можно получить неверные данные
	T& GetFrontValue() { return _buffer[_begin]; }
	
	/// Возвращает ссылку на последний элемент буфера. В случае, если буфер переполнялся, то индекс последнего элемента проходит переадресацию на буфере
	/// \Warning Нет привязки к размеру буфера, в случае, если буфер пустой, можно получить неверные данные
	T& GetBackValue() { return _buffer[_end]; }

	/// Устанавливает значение элемента по индексу. В случае, если буфер переполнялся, то индекс проходит переадресацию на буфере
	void SetValue(int index, T value) { GetValue(index) = value; }

	/// Возвращает константную ссылку на элемент буфера по индексу. В случае, если буфер переполнялся, то индекс проходит переадресацию на буфере
	/// \Warning Нет привязки к размеру буфера, в случае, если буфер пустой, можно получить неверные данные
	const T& GetValue(int index) const { return _buffer[(_begin + index) % _capacity]; }

	/// Возвращает константную ссылку на первый элемент буфера. В случае, если буфер переполнялся, то индекс первого элемента проходит переадресацию на буфере
	/// \Warning Нет привязки к размеру буфера, в случае, если буфер пустой, можно получить неверные данные
	const T& GetFrontValue() const { return _buffer[_begin]; }

	/// Возвращает константную ссылку на последний элемент буфера. В случае, если буфер переполнялся, то индекс последнего элемента проходит переадресацию на буфере
	/// \Warning Нет привязки к размеру буфера, в случае, если буфер пустой, можно получить неверные данные
	const T& GetBackValue() const { return _buffer[_end]; }

	/// Удаляет последний элемент в буфере
	void EraseBack()
	{
		if (!IsEmpty())
		{
			_end = (_end - 1 + _capacity) % _capacity;
			--_size;
		}
	}

	/// Удаляет первый элемент в буфере
	void EraseFront()
	{
		if (!IsEmpty())
		{
			_begin = (_begin + 1) % _capacity;
			--_size;
		}
	}

	/// Добавляет элемент в конец буфера
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

	/// Добавляет элемент в начало буфера
	void AddValueFront(T value)
	{
		if (!IsEmpty())
			_begin = (_begin - 1 + _capacity) % _capacity;
		_buffer[_begin] = value;
		_end = (_begin + _size) % _capacity;
		_size = (_size + 1 > _capacity) ? _capacity : (_size + 1);
	}

	/// Очищает буфер
	void Clear()
	{
		memset(_buffer, 0, _capacity * sizeof(T));
		_begin = 0;
		_end = 0;
		_size = 0;
	}
};

#endif // CIRCLEBUFFER_H
