# /*
#  * This file is part of the [Apollo] project.
#  * Copyright [2023] [Rui Zhang] and other contributors.
#  * See the LICENSE file in the project root directory for the full license information.
#  * 
#  * The sharing rights for this project belong to all those who have contributed to the project.
#  * 
#  * Thanks to the following contributors for their dedication and support of this project:
#  *   - Rui Zhang 
# */

$licenseFile = "../LICENSE"
$licenseFolder = "../license"

# Determine if the LICENSE file exists, and delete it if it does.
if (Test-Path $licenseFile) 
{
    Remove-Item $licenseFile -Force
    Write-Host "LICENSE file has been deleted."
} else 
{
    Write-Host "LICENSE file does not exist."
}

# Determine if the license folder exists and create it if it does not exist.
if (-not (Test-Path $licenseFolder)) 
{
    New-Item -ItemType Directory -Path $licenseFolder
    Write-Host "license folder has been created."
} else 
{
    Write-Host "license folder already exists."
}

cp ../LICENSE-Apollo ../license/LICENSE-Apollo
cp ../LICENSE-Apollo-LS ../license/LICENSE-Apollo-LS

