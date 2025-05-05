int some_var = 5;
const double DO_NOT_PRINT_ME = some_var;

double sum(int a, float b) {
	return a + b;
}

int mul(float a, float b) {
	return a + sum(a, b);
}

/*
Function `sum`
int -> float: 1
float -> double: 1

Function `mul`
float -> int: 1
float -> double: 1
double -> int: 1
*/
