@echo off
FOR /r %%X IN (*.fx) DO (
	%1 %%X %%Xc < %%X
)
