$myPath = Get-Location
Write-Host "Will delete tmp directories from: $myPath" -ForegroundColor Green

# Define an array of inventory directory names to delete
$inventory = @('.vs', '.vsconfig', 'Binaries', 'DerivedDataCache'
, 'Intermediate', 'Saved'
, 'Plugins\BA_RepArray\Intermediate', 'Plugins\BA_RepArray\Binaries')

# Iterate over the array and delete each inventory directory if it exists
foreach ($item in $inventory) {
    if (Test-Path $item) {
        Write-Host "Deleting directory '$item' with all subdirectories and files..." -ForegroundColor Green
        Remove-Item $item -Recurse -Force
    } else {
        Write-Host "Directory '$item' does not exist." -ForegroundColor DarkYellow
    }
}

# delete sln file
foreach ($file in Get-ChildItem -Filter "*.sln") {
	Write-Host "Deleting Visual Studio solution file '$item'" -ForegroundColor Green
	Remove-Item $file -Force
}
  
# Add a pause at the end of the script
Write-Host "Press Enter to exit script" -ForegroundColor White
Read-Host
