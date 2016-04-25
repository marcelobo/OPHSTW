@ECHO OFF
for /L %%i IN (0, 1, 7) DO (
	for /L %%h IN (1, 1, 3) DO (
		for /L %%e IN (1, 1, 10) DO (
			OPHSTW.exe %%i %%h %%e
		)
	)	
)
pause