#!/bin/bash

IFS='' read -r -d '' DECODER <<"EOF"
#!/bin/sh
tail -c +113 $0 > /tmp/new.gz
gunzip -f /tmp/new.gz
chmod +x /tmp/new
exec /tmp/new
rm /tmp/new
exit
EOF

echo "$DECODER" > $2
gzip -c $1 >> $2
chmod +x $2
