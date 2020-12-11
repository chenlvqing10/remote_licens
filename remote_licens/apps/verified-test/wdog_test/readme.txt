wdog_test:
	test cmd :"./wdog_test"
	test directory :"/usr/bin"

	It feeds dog every 16s.After running the program:
	if you do nothing,the system will reboot 32s(two 16s) later;
	if you press 's' within 6s,the system will reboot 10s later;
	if you press 's' between 7s and 16s,the system will reboot 26s
	(16s + 10s) later;
	if you don't press 'd' before pressing 'q',after pressing 'q' for
	16s,the system will reboot.

	In n7,the timeout can only belong to the collection {0.5, 1, 2, 3,
	4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 16}(the uint is second);
	In n8,the timeout can be set as a int variable in the range-[1, 524000]
	(the uint is millisecond).
