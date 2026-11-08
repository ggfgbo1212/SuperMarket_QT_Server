OBJS=sql_connect.o myserve.o adim_thread.o main.o adim.o mcu_thread.o mcu.o cus.o cus_thread.o

test: $(OBJS)
	gcc $^ -o $@ -lmysqlclient -lpthread

sql_connect.o:sql_connect.c
	gcc -c $< -o $@ -I/usr/include/mysql


myserve.o: myserve.c  	
	gcc -c $< -o $@ -I/usr/include/mysql 


adim_thread.o: adim_thread.c  	
	gcc -c $< -o $@ -I/usr/include/mysql


main.o: main.c  	
	gcc -c $< -o $@ -I/usr/include/mysql


adim.o: adim.c  	
	gcc -c $< -o $@ -I/usr/include/mysql


mcu_thread.o: mcu_thread.c  	
	gcc -c $< -o $@ -I/usr/include/mysql


mcu.o:mcu.c  	
	gcc -c $< -o $@ -I/usr/include/mysql


cus.o:cus.c  
	gcc -c $< -o $@ -I/usr/include/mysql



cus_thread.o:cus_thread.c  	
	gcc -c $< -o $@ -I/usr/include/mysql

clean:
	rm *.o test
