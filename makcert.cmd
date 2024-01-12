rem
rem Thanks! 
rem https://www.digitalocean.com/community/tutorials/openssl-essentials-working-with-ssl-certificates-private-keys-and-csrs
rem

rem rootca. used by client
openssl genrsa -out certs/rootca.key 4096
openssl req -x509 -new -nodes -key certs/rootca.key -days 20000 -out certs/rootca.pem -subj "/C=GB/ST=London/L=London/O=CompanyName/CN=root"

openssl genrsa -out certs/user.key 4096
openssl req -new -key certs/user.key -out certs/user.csr -subj "/C=GB/ST=London/L=London/O=CompanyName/CN=server"

openssl x509 -req -in certs/user.csr -CA certs/rootca.pem -CAkey certs/rootca.key -CAcreateserial -out certs/user.pem -days 20000

rem final check will fail.
openssl verify -CAfile certs/rootca.pem certs/rootca.pem
openssl verify -CAfile certs/rootca.pem certs/user.pem
openssl verify -CAfile certs/user.pem certs/user.pem

rem for exchange. this is slw to generate
openssl dhparam -out certs/dh4096.pem 4096

