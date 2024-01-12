rem
rem
rem

openssl genrsa -out certs/rootca.key 4096

rem Root. used by client
openssl req -x509 -new -nodes -key certs/rootca.key -days 20000 -out certs/rootca.pem

openssl genrsa -out certs/user.key 4096
openssl req -new -key certs/user.key -out certs/user.csr

openssl x509 -req -in user.csr -CA certs/rootca.pem -CAkey certs/rootca.key -CAcreateserial -out certs/user.pem -days 20000

openssl verify -CAfile certs/rootca.pem certs/rootca.pem
openssl verify -CAfile certs/rootca.pem certs/user.pem
openssl verify -CAfile certs/user.pem certs/user.pem

rem for exchange
openssl dhparam -out certs/dh4096.pem 4096

