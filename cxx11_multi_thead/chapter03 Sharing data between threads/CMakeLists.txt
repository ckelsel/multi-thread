add_executable(01_list_with_mutex 01_list_with_mutex.cpp)
add_test(NAME 01_list_with_mutex
    COMMAND $<TARGET_FILE:01_list_with_mutex>)

add_executable(02_pass_to_user_func 02_pass_to_user_func.cpp)
add_test(NAME 02_pass_to_user_func
    COMMAND $<TARGET_FILE:02_pass_to_user_func>)

add_executable(03_std_lock_avoid_deadlock 03_std_lock_avoid_deadlock.cpp)
add_test(NAME 03_std_lock_avoid_deadlock
    COMMAND $<TARGET_FILE:03_std_lock_avoid_deadlock>)

add_executable(04_resource_init 04_resource_init.cpp)
add_test(NAME 04_resource_init
    COMMAND $<TARGET_FILE:04_resource_init>)

add_executable(05_thread_safe_lazy_initialization_of_a_class_member 05_thread_safe_lazy_initialization_of_a_class_member.cpp)
add_test(NAME 05_thread_safe_lazy_initialization_of_a_class_member
    COMMAND $<TARGET_FILE:05_thread_safe_lazy_initialization_of_a_class_member>)
