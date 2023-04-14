Write-Output "[DEBUG BEGIN] Starting program..."
& ".\program_release.exe"
if (0 -ne $LastExitCode) {
	Write-Error -Message "[ERROR] Exit status $LastExitCode" -CategoryTargetName "program_debug.exe" -CategoryReason "Program did not exit successfully!" -RecommendedAction "Please check your program with a debugger to find where it happens." -Category InvalidResult
	Write-Output ""
} else {
	Write-Information "Exit status OK"
	Write-Output ""
}
Write-Output "[DEBUG END] Please press ANY KEY to close program."
[console]::ReadKey()