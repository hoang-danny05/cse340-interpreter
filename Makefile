
it:
	rm -f a.out
	g++ execute.cc inputbuf.cc lexer.cc demo.cc

echo: 
	cat ./provided_tests/test_assignment_basic2.txt


test1: 
	make it 
	./a.out < ./provided_tests/test_assignment_basic2.txt

test2: 
	make it 
	./a.out < ./provided_tests/test_assignment_basic1.txt
	./a.out < ./provided_tests/test_assignment_variables_no_init.txt
	./a.out < ./provided_tests/test_assignment_variables1.txt
	./a.out < ./provided_tests/test_assignment_variables2_operators.txt


testif: 
	make it
	./a.out < provided_tests/test_control_i_if_f2.txt

testif2:
	./a.out < ./provided_tests/test_control_i_if_f_i_if_f1.txt