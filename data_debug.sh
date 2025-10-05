#!/bin/bash
# Simple test to isolate the issue

echo "Testing DATA statement recognition..."
echo '10 DATA 42, "hello", 3.14' | prismshell

echo ""
echo "Testing READ statement recognition..."
echo '10 DATA 42
20 READ x' | prismshell

echo ""
echo "Testing RESTORE statement recognition..."
echo '10 DATA 42
20 READ x
30 RESTORE' | prismshell

echo ""
echo "Complete simple test..."
echo '10 DATA 42
20 READ x
30 PRINT x' | prismshell