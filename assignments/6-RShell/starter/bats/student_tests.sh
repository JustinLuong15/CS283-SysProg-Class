#!/usr/bin/env bats

# File: student_tests.sh
# 
# Create your unit tests suit in this file

setup() {
    TEST_DIR=$(mktemp -d)
    mkdir -p "$TEST_DIR/subdir"
}

teardown() {
    rm -rf "$TEST_DIR"
}

@test "Example: check ls runs without errors" {
    run ./dsh <<EOF                
ls
EOF

    # Assertions
    [ "$status" -eq 0 ]
}

@test "Check pwd runs without errors" {
    run ./dsh <<EOF                
pwd
EOF
    [ "$status" -eq 0 ]
    [ -n "$output" ] 
}

@test "Check echo command" {
    run ./dsh <<EOF
echo Hello, dsh!
EOF

    echo "Debug Output: '$output'"
    [[ "$output" =~ .*Hello,\ dsh!.* ]]
}

@test "Check cd command changes directory" {
    run ./dsh <<EOF                
cd /tmp
pwd
EOF
    [[ "$output" == "/tmp"* ]]
}

@test "Check exit command" {
    run ./dsh <<EOF                
exit
EOF
    [ "$status" -eq 0 ]
}

@test "Check invalid command returns error" {
    run ./dsh <<EOF
invalid_command
EOF

    echo "Debug Status: $status"
    echo "Debug Output: '$output'"
    
    [[ "$status" -ne 0 ]] || [[ "$output" =~ "Command not found" ]]
}

@test "Check remote command execution" {
    run ./dsh <<EOF                
rc
EOF

    echo "Debug Output: '$output'"

    [ "$status" -eq 0 ]
    [[ "$output" =~ .*0.* ]] 
}

@test "Client-server connection attempt" {
    PORT=$((RANDOM % 50000 + 10000))
    SERVER_LOG=$(mktemp)
    CLIENT_LOG=$(mktemp)
    
    ./dsh -s -i 127.0.0.1 -p $PORT > "$SERVER_LOG" 2>&1 &
    server_pid=$!

    for i in {1..10}; do
        if grep -q "Server is listening" "$SERVER_LOG"; then
            break
        fi
        sleep 0.5
    done
    
    run timeout 5s ./dsh -c -i 127.0.0.1 -p $PORT <<EOF
echo test-connection
exit
EOF
    client_status=$?
    
    kill -TERM $server_pid 2>/dev/null || true
    wait $server_pid 2>/dev/null || true
    
    echo "=== Server Log ==="
    cat "$SERVER_LOG"
    echo "=== Client Output ==="
    echo "$output"
    
    rm "$SERVER_LOG" "$CLIENT_LOG"
    
    [ "$client_status" -eq 0 ]
    [[ "$output" == *"test-connection"* ]]
}

@test "Test empty command" {
    run ./dsh <<< ""
    [ "$status" -eq 0 ]
    [[ "$output" == *"warning: no commands provided"* ]]
}

@test "Test exit command" {
    run ./dsh <<< "exit"
    [ "$status" -eq 0 ]
    [[ "$output" == *"cmd loop returned"* ]]
}

@test "Test cd command with existing directory" {
    run ./dsh <<EOF
cd $TEST_DIR/subdir
pwd
exit
EOF
    echo "$output"
    [ "$status" -eq 0 ]
    [[ "$output" == *"/subdir"* ]]
}

@test "Test cd command with invalid directory" {
    run ./dsh <<< "cd /invalid/path"
    [ "$status" -eq 0 ]
    [[ "$output" == *"cd: No such file or directory"* ]]
}

@test "Test cd with too many arguments" {
    run ./dsh <<< "cd dir1 dir2"
    [ "$status" -eq 0 ]
    [[ "$output" == *"too many arguments"* ]]
}

@test "Test external command execution" {
    run ./dsh <<< "ls -l"
    [ "$status" -eq 0 ]
    [[ "$output" == *"dsh"* ]] 
}

@test "Test command with quoted arguments" {
    run ./dsh <<< 'echo "  hello   world  "'
    echo "$output"
    [ "$status" -eq 0 ]
    [[ "$output" == *"  hello   world  "* ]]
}

@test "Test command not found" {
    run ./dsh <<< "nonexistent_command"
    [ "$status" -eq 0 ]
    [[ "$output" == *"Command not found in PATH"* ]]
}

@test "Test rc command with success status" {
    run ./dsh <<EOF
ls
rc
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == *"0"* ]]
}

@test "Test rc command with error status" {
    run ./dsh <<EOF
cd /invalid/path
rc
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == *"2"* ]]
}

@test "Test multiple commands" {
    run ./dsh <<EOF
cd $TEST_DIR
pwd
ls
rc
exit
EOF
    echo "$output"
    [ "$status" -eq 0 ]
    [[ "$output" == *"$TEST_DIR"* ]]
    [[ "$output" == *"subdir"* ]]
    [[ "$output" == *"0"* ]]
}

@test "Test command with multiple spaces" {
    run ./dsh <<< "   ls   -l   "
    [ "$status" -eq 0 ]
    [[ "$output" == *"dsh"* ]] 
}

@test "Test command with mixed quotes" {
    run ./dsh <<< "echo 'single quoted   string'"
    [ "$status" -eq 0 ]
    [[ "$output" == *"single quoted   string"* ]]
}