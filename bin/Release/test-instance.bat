@ECHO OFF
for /L %%e IN (1, 1, 10) DO (
	OPHSTW.exe 0 1 %%e
)
pause