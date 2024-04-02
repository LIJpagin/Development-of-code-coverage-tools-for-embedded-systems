#include <iostream>
#include <map>
#include <set>
#include <vector>

void foo() { }

void foo()
{ }

void foo() {
}

void foo() {

}

void foo()
{
}

void foo()
{
	int i = 1;
}

template<typename T>
void foo1() { }

template<typename T>
void foo2()
{ }

template<typename T>
void foo3()
{ }

template<typename T>
void foo4()
{

}

template<typename T>
void foo5()
{ }

template<typename T>
void foo6()
{
	int i = 1;
}

class h
{
	void foo1();
	void foo() { }
	
	void foo() {
	}
	
	void foo(){
	
	}
	
	void foo()
	{
	
	}
};

void h::foo1() { }

int main()
{
	foo1();
	foo2();
	foo3();
	foo4();
	foo5();
	foo6();

	int i = 0;
	if (i == 1) i = 1;

	if (i == 1)
		i = 1;

	if (i == 1) { i = 1; }

	if (i == 1) {
		i = 1; }

	if (i == 1) {
		i = 1;
	}

	if (i == 1)
	{
		i = 1;
	}

	if (i == 1) {
		i = 1;
		i = 1;
		i = 1;
	}

	if (i == 1)
	{
		i = 1;
		i = 1;
		i = 1;
	}

	if (i == 1) i = 1;
	else i = 2;

	if (i == 1)
		i = 1;
	else i = 2;

	if (i == 1)i = 1;
	else
		i = 2;

	if (i == 1)
		i = 1;
	else
		i = 2;

	if (i == 1) { i = 1; }
	else { i = 2; }

	if (i == 1) {
		i = 1; }
	else {
		i = 2; }

	if (i == 1) {
		i = 1; }
	else { i = 2; }

	if (i == 1) { i = 1; }
	else {
		i = 2; }

	if (i == 1)
	{
		i = 1; }
	else
	{
		i = 2; }

	if (i == 1) {
		i = 1;
	}
	else {
		i = 2;
	}

	if (i == 1)
	{
		i = 1;
	} else
	{
		i = 2;
	}

	if (i == 1) {
		i = 1;
	} else {
		i = 2;
	}

	if (i == 1) {
		i = 1;
		i = 1;
		i = 1;
	}
	else {
		i = 2;
		i = 2;
		i = 2;
	}

	if (i == 1)
	{
		i = 1;
		i = 1;
		i = 1;
	}
	else
	{
		i = 2;
		i = 2;
		i = 2;
	}

	if (i == 1) i = 1;
	else if (i == 2) i = 2;

	if (i == 1)
		i = 1;
	else if (i == 2) i = 2;

	if (i == 1) i = 1;
	else if (i == 2)
		i = 2;

	if (i == 1) 
		i = 1;
	else if (i == 2)
		i = 2;

	if (i == 1)
		i = 1;
	else
		if (i == 2)
			i = 2;

	if (i == 1) { i = 1; }
	else if (i == 2) { i = 2; }

	if (i == 1) {
		i = 1; }
	else if (i == 2) {
		i = 2; }

	if (i == 1) {
		i = 1;
	}
	else if (i == 2) {
		i = 2;
	}

	if (i == 1)
	{
		i = 1;
		i = 1;
		i = 1;
	}
	else if (i == 2)
	{
		i = 2;
		i = 2;
		i = 2;
	}


	if (i == 1) i = 1;
	else if (i == 2) i = 2;
	else if (i == 3) i = 3;
	else if (i == 4) i = 4;
	else if (i == 5) i = 5;

	if (i == 1)
		i = 1;
	else if (i == 2)
		i = 2;
	else if (i == 3)
		i = 3;
	else if (i == 4)
		i = 4;
	else if (i == 5)
		i = 5;

	if (i == 1)
	{
		i = 1;
	}
	else if (i == 2)
	{
		i = 2;
	}
	else if (i == 3)
	{
		i = 3;
	}
	else if (i == 4)
	{
		i = 4;
	}
	else if (i == 5)
	{
		i = 5;
	}

	while (true)
		;

	while (true) i++;

	while (true)
		i++;

	while (true) { i++; }

	while (true) {
		i++;
	}

	while (true)
	{
		i++;
	}

	while (true)
	{
		i++;
		i++;
		i++;
	}

	for (;;)
		;

	for (;;) i++;

	for (;;)
		i++;

	for (;;) { i++; }

	for (;;) {
		i++;
	}

	for (;;)
	{
		i++;
	}

	for (;;)
	{
		i++;
		i++;
		i++;
	}

	for (int q = 0; q < 10; ++q)
	{
		i++;
	}

	do i++;
	while (true);

	do
		i++;
	while (true);

	do { i++; }
	while (true);

	do {
		i++;
	} while (true);

	do {
		i++;
	}
	while (true);

	do
	{
		i++;
	}
	while (true);

	do
	{
		i++;
		i++;
		i++;
	} while (true);

	bool c = i == 1 ? 1 : 2;

	if (i == 1 ? 1 : 2)
		i++;

	switch (i)
	{
		case 1: i++;
		case 2: i++;
		case 3: i++;
		case 4: i++;
	}

	switch (i)
	{
		case 1: i++; break;
		case 2: i++; break;
		case 3: i++; break;
		case 4: i++; break;
	}

	switch (i)
	{
		case 1:
			i++;
			break;
		case 2:
			i++;
			break;
		case 3:
			i++;
			break;
		case 4:
			i++;
			break;
	}

	switch (i)
	{
		case 1:
			{
				i++;
				i++;
				i++;
			}
			break;
		case 2:
			{
				i++;
				i++;
				i++;
			}
			break;
		case 3:
			{
				i++;
				i++;
				i++;
			}
			break;
		case 4:
			{
				i++;
				i++;
				i++;
			}
			break;
	}

	switch (i)
	{
		case 1:
			{
				i++;
				i++;
				i++;
				break;
			}
		case 2:
			{
				i++;
				i++;
				i++;
				break;
			}
		case 3:
			{
				i++;
				i++;
				i++;
				break;
			}
		case 4:
			{
				i++;
				i++;
				i++;
				break;
			}
	}

	[] () { };

	[] () {
		};

	[] ()
		{ };

	int w = [] () { };

	int e = [] () {
		};

	int r = [] ()
		{
		};

	int t = [] ()
		{
		
		};
}