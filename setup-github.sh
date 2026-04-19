#!/bin/bash
# Setup script for RISC-V emulator GitHub repository

set -e

echo "=== RISC-V Emulator GitHub Setup ==="

# Check if remote is already set
if git remote get-url origin &>/dev/null; then
    echo "Remote 'origin' is already set."
    REMOTE_URL=$(git remote get-url origin)
    echo "Current remote URL: $REMOTE_URL"
else
    echo "Setting remote 'origin'..."
    git remote add origin https://github.com/paraschen/riscv-emulator.git
fi

echo ""
echo "To push to GitHub, you need to:"
echo "1. Create a repository on GitHub:"
echo "   - Go to https://github.com/new"
echo "   - Repository name: riscv-emulator"
echo "   - Description: 'Complete RISC-V system emulator in C++'"
echo "   - Public repository"
echo "   - DO NOT initialize with README, .gitignore, or license"
echo "   - Click 'Create repository'"
echo ""
echo "2. After creating the repository, run:"
echo "   git push -u origin main"
echo ""
echo "3. If authentication is required, use your GitHub token as password:"
echo "   Token: "
echo ""
echo "Alternative push command with token embedded:"
echo "   git push https://@github.com/paraschen/riscv-emulator.git main"
echo ""
echo "Project is ready at: $(pwd)"