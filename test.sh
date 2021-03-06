#!/bin/bash
assert() {
	expected="$1"
	input="$2"

	./9cc "$input" > tmp.s
	cc -o tmp tmp.s
	./tmp
	actual="$?"

	if [ "$actual" = "$expected" ]; then
		echo "$input => $actual"
	else
		echo "$input => $expected. but got $actual"
		exit 1
	fi
}

assert 0 '0;'
assert 42 '42;'
assert 21 "5+20-4;"
assert 41 " 12 + 34 - 5; "
assert 47 '5+6*7;'
assert 15 '5*(9-6);'
assert 4 '(3+5)/2;'
assert 10 '-3 + 13;'
assert 10 '+3 + (4 + 3);'
assert 10 '-(3 + 10) + 23;'
assert 1 '(3 + 5) == 8;'
assert 0 '(3 + 5) != 8;'
assert 1 '4 <= 5;'
assert 0 '4 >= 5;'
assert 3 '(5 < 4) + 3 * (2 > 1);'
assert 4 'a = 2; b = 4; a * b - (1 + 3);'
assert 5 'var = 3; tmp = var * 2; tmp - 1;'
assert 3 'var = 3; return var; var + 2;'

echo OK