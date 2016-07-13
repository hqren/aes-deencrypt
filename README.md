# aes-deencrypt

### How to use? ###
````
bash> hdfs dfs -put libaes-deencrypt.so /udf/libaes-deencrypt.so

impala> create function udf.aes_decrypt(string,string) returns string location '/udf/libaes-deencrypt2.0.so' symbol='AESDecrypt' PREPARE_FN='AESPrepare' CLOSE_FN='AESClose';

impala> select udf.aes_decrypt(unhex('CBA4ACFB309839BA426E07D67F23564F'), '1234567890123456');

impala> select udf.aes_decrypt(unhex('EB02EC381CA0145F5320A458A899B5A9'), '1234567890');

impala> select udf.aes_decrypt(unhex(''), '1234567890123456');

impala> create function udf.aes_encrypt(string,string) returns string location '/udf/libaes-deencrypt2.0.so' symbol='AESEncrypt' PREPARE_FN='AESPrepare' CLOSE_FN='AESClose';

impala> select hex(udf.aes_encrypt('ABC', '1234567890123456'));

impala> select hex(udf.aes_encrypt('ABC', '1234567890'));

impala> select hex(udf.aes_encrypt('', '1234567890123456'));

bash> hdfs dfs -put aes-deencrypt.ll /udf/aes-deencrypt.ll

impala> create function aes_decrypt(string,string) returns string location '/udf/aes-deencrypt.ll' symbol='AES_Decrypt';

impala> select udf.aes_decrypt(unhex('CBA4ACFB309839BA426E07D67F23564F'), '1234567890123456')
;

impala> create function udf.aes_encrypt(string,string) returns string location '/udf/aes-deencrypt.ll' symbol='AES_Encrypt';

impala> select hex(aes_encrypt('ABC', '1234567890123456'));
````

