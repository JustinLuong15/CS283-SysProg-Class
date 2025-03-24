#!/usr/bin/env bats

# File: student_tests.sh
# 
# Create your unit tests suit in this file

setup() {
  ./dsh -s -p 1234 &
  SERVER_PID=$!
  sleep 1
}

teardown() {
  if kill -0 "$SERVER_PID" 2>/dev/null; then
    kill "$SERVER_PID"
    wait "$SERVER_PID" 2>/dev/null || true
  fi
}

@test "Local mode: Check ls runs without errors" {
    run ./dsh <<EOF                
ls
EOF

    [ "$status" -eq 0 ]
}

@test "Local mode: Check echo command works" {
    run ./dsh <<EOF
echo "local mode"
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == *"local mode"* ]]
}

@test "Remote mode: Echo command works" {
    run ./dsh -c -p 1234 <<EOF
hello world
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == *"hello world"* ]]
}

@test "Remote mode: Exit command closes connection" {
    run ./dsh -c -p 1234 <<EOF
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == *"exit"* ]]
}

@test "Remote mode: stop-server command shuts down server" {
    run ./dsh -c -p 1234 <<EOF
stop-server
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == *"stop-server"* ]]
}

@test "Remote mode: Multiple commands in sequence" {
    run bash -c 'printf "first\nexit\n" | ./dsh -c -p 1234'
    [ "$status" -eq 0 ]
    [[ "$output" == *"first"* ]]
    [[ "$output" == *"exit"* ]]
}

@test "Remote mode: Pipeline command works" {
    run ./dsh -c -p 1234 <<EOF
echo hello world | rc
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == *"0"* ]]
}

