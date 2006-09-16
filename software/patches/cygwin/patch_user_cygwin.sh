# create new users for rootfs generation
if grep "root:\*:0:0:,S-1-5-32-0::" /etc/passwd >/dev/null 2>&1; then \
	echo "root already present in etc/passwd"
else
	echo "root:*:0:0:,S-1-5-32-0::" >>/etc/passwd
fi;

if grep "user1000:\*:1000:1000:,S-1-5-32-1000::" /etc/passwd >/dev/null 2>&1; then \
	echo "user1000 already present in etc/passwd"
else
	echo "user1000:*:1000:1000:,S-1-5-32-1000::" >>/etc/passwd
fi;

#create new groups for rootfs generation
if grep 'root:S-1-5-32-0:0:' /etc/group >/dev/null 2>&1; then \
	echo "root already present in etc/group"
else
	echo "root:S-1-5-32-0:0:" >>/etc/group
fi;

if grep 'root5:S-1-5-32-5:5:' /etc/group >/dev/null 2>&1; then \
	echo "root5 already present in etc/group"
else
	echo "root5:S-1-5-32-5:5:" >>/etc/group
fi;

if grep 'user1000:S-1-5-32-1000:1000:' /etc/group >/dev/null 2>&1; then \
	echo "user1000 already present in etc/group"
else
	echo "user1000:S-1-5-32-1000:1000:" >>/etc/group
fi;

	