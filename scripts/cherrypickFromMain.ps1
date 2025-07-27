Param (
    [Parameter(Mandatory=$false)]
    [String] $commit
)

Write-Output "Reminder: Use `"xxxxx~..xxxxx`" for a range A through B"

If ($commit.Length -eq 0) {
    $commit = git rev-parse main
    Write-Output "Cherry-picking commit $commit"
}

$branches = @(
    "bs-1.37.0",
    "bs-1.40.4",
    "bs-1.40.6"
)

ForEach ($branch in $branches) {
    git checkout $branch
    If ($LASTEXITCODE) {
        Write-Output "Exiting script"
        exit;
    }

    git cherry-pick $commit
    If ($LASTEXITCODE) {
        Write-Output "Exiting script"
        exit;
    }
}

git checkout main

Write-Host "REMINDER: git push --all" -ForegroundColor Yellow