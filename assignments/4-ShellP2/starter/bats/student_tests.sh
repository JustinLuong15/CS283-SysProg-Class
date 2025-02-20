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