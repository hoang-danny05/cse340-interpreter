
it:
	rm -f a.out
	g++ execute.cc inputbuf.cc lexer.cc demo.cc

echo: 
	cat ./provided_tests/test_assignment_basic2.txt


test1: 
	make it 
	./a.out < ./provided_tests/test_assignment_basic2.txt

