param(
    [Parameter(Mandatory = $true)][string]$Uv4,
    [Parameter(Mandatory = $true)][string]$Project,
    [Parameter(Mandatory = $true)][string]$Log
)

& $Uv4 -b $Project -t hao -j0 -o $Log
$exitCode = $LASTEXITCODE
$output = if (Test-Path -LiteralPath $Log) { Get-Content -LiteralPath $Log -Raw } else { "" }
if ($exitCode -gt 1 -or $output -notmatch '0 Error\(s\)') { exit 2 }
exit 0
