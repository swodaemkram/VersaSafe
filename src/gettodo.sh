#!/bin/bash
echo "Deleting old todo.txt file"
rm -f todo.txt
grep -rn "TODO" * > todo.txt
echo "Done!"
