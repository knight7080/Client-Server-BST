all: tst_client server_bst

tst_client: tst_client.c 
	gcc tst_client.c -o tst_client

server_bst: tst_server.c up_bst.c 
	gcc tst_server.c up_bst.c -o server_bst

clean:
	rm tst_client server_bst

clean_file:
	rm bst_file file_store