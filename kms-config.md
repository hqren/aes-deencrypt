hadoop create key "decrypt.k1


[localhost:21000] > select hex(encrypt('ABC'));                               
Query: select hex
<table border="1">
<tr>
<th> hex(encrypt('abc'))  </th>
</tr>
<tr>
<td>9EB6070CA745C1A6EC8F2978465838BD</td>
</tr>
</table>

[localhost:21000] > select decrypt(unhex('9EB6070CA745C1A6EC8F2978465838BD'));
Query: select decrypt(unhex('9EB6070CA745C1A6EC8F2978465838BD'))
<table border="1">
<tr>
<th> decrypt(unhex('9eb6070ca745c1a6ec8f2978465838bd')) </th>
</tr>
<tr>
<td>ABC</td>
</tr>
</table>
