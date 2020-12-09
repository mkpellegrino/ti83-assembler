# ti83-assembler

This program will assemble special asm source code into an 8xp file that can be executed directly on a TI-83/84 calculator.
There is no need for any kind of conversion program or special shell to be running on the Calculator

compile asm source with:
tias input.asm output.8xp

there are some built-in macros:
loop
user_input
degree_mode
radian_mode
disp_op1 (a 'better' routine than the built-in one)
fully_clear_screen
convop1b (a 'better' routine than the built-in one)
store_op1
