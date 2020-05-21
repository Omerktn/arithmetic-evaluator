
# arithmetic-evaluator
An arithmetic expression evaluation program.

![Screenshot](ss.png)
<br/>

Operations:
| Symbol                              | Description                        |
|-------------------------------------|------------------------------------|
| * / + -            | Basic arithmetic operations      |
| ^ | Power |
<br/>

Functions:
| Syntax                              | Description                        |
|-------------------------------------|------------------------------------|
| sin() cos() tan() cot()             | Core trigonometric functions       |
| arcsin() arccos() arctan() arccot() | Inverse of trigonometric functions |
| sqrt()                              | Square root                        |
| floor() round() ceil()              | Standart floor, round, ceil functions|
| log() ln()                          | log_10 and log_e (natural log.) functions|
<br/>

Constants:
| Symbol                              | Description                        |
|-------------------------------------|------------------------------------|
| e             | euler number      |
| pi | pi number |
| m                              | (memory) former answer                        |
<br/>

Run standalone:

	$ make
	$ ./eval
Run tests:

	$ make test
	$ ./test

Floating point rounding error: 1 in a 10^9
