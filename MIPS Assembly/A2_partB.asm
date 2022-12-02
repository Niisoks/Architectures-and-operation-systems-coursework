.text
	main:
		li $t0, 0 # number 1 of fibonacci
		li $t1, 1 # number 2 of fibonacci
		li $t2, 0 # SUM of no.1 and no.2
		li $t3, 0 # i=0
		li $t4, 5 # number of loops
		
	for:
		beq $t3, $t4, end #go to end when t3 = t4
		li $v0, 1 # preps syscall to print immediate
		move $a0, $t0 #moves t0 to a0
		syscall # print t0 to console
		li $v0, 11  # syscall number for printing character
		li $a0, 32 # ascii code for space to give gap between numbers
		syscall# print to console
		add $t2, $t0, $t1 # add t0 and t1 to t2
		add $t0, $t1, $zero # changes t0 to t1
		add $t1, $t2, $zero # changes t1 to t2
		addi $t3, $t3, 1 # add 1 to t3
		j	for#jump to top of for loop

	end:
		#terminate program 
		li $v0 10# value 10 is good for exiting program
		syscall# print to console
