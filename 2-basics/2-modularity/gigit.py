#!/usr/bin/env python3

import argparse
import tempfile
import shutil
import subprocess
import os
import sys
import contextlib
import shutil
from pathlib import Path

# Copied from https://stackoverflow.com/a/42441759
@contextlib.contextmanager
def working_directory(path):
    """Changes working directory and returns to previous on exit."""
    prev_cwd = Path.cwd()
    os.chdir(path)
    try:
        yield
    finally:
        os.chdir(prev_cwd)


parser = argparse.ArgumentParser(description="Git in git")
parser.add_argument("folders", nargs="+", type=str, help="Folders")

if __name__ == "__main__":
    args = parser.parse_args()
    folders = args.folders
    if len(folders) < 2:
        print("At least two folders have to be specified. Abort")
        sys.exit(1)

    git_repo_root = (
        subprocess.check_output(["git", "rev-parse", "--show-prefix"])
        .decode(encoding="utf-8")
        .rstrip()
    )

    changed_files = (
        subprocess.check_output(["git", "diff", "--name-only", "HEAD"])
        .decode(encoding="utf-8")
        .rstrip()
    )

    changed_folder = None
    for folder in folders:
        if os.path.normpath(os.path.join(git_repo_root, folder)) in changed_files:
            if changed_folder is None:
                changed_folder = folder
            else:
                print("Only one changed folder is supported. Abort.")
                sys.exit(1)

    if changed_folder is None:
        print("No folder among the passed ones has been changed. Finish.")
        sys.exit()
    if changed_folder == folders[len(folders) - 1]:
        print("Changed folder is the last one. No need to do anything. Finish.")
        sys.exit()

    print(f"Changed folder: {changed_folder}. Proceed with hacky rebase")

    subprocess.check_output(["git", "stash", "push", "--", changed_folder])
    print(f"Stashed changes in {changed_folder}")

    tmp_dir = tempfile.mkdtemp()
    print(f"Initializing tmp git repository in {tmp_dir}")
    with working_directory(tmp_dir):
        subprocess.check_output(["git", "init"])

    changed_folder_commit_sha = None
    for folder in folders:
        shutil.copytree(
            folder,
            tmp_dir,
            dirs_exist_ok=True,
        )
        with working_directory(tmp_dir):
            subprocess.check_output(["git", "add", "."])
            subprocess.check_output(["git", "commit", ".", "-m", folder])
            commit_sha = (
                subprocess.check_output(["git", "rev-parse", "HEAD"])
                .decode(encoding="utf-8")
                .rstrip()
            )
            if folder == changed_folder:
                changed_folder_commit_sha = commit_sha

            print(f"Applied {folder} ({commit_sha})")

    subprocess.check_output(["git", "stash", "pop"])
    print(f"Unstashed changes in {changed_folder}")

    with working_directory(tmp_dir):
        subprocess.check_output(
            ["git", "checkout", "-b", "changed", changed_folder_commit_sha]
        )
    shutil.copytree(
        changed_folder,
        tmp_dir,
        dirs_exist_ok=True,
    )
    with working_directory(tmp_dir):
        subprocess.check_output(["git", "add", "."])
        subprocess.check_output(["git", "commit", "--amend", "--no-edit"])
        print(f"Applied changed folder {changed_folder}")

        subprocess.check_output(["git", "checkout", "master"])

        print(f"Start rebasing")
        rebase_has_conflicts = False
        try:
            subprocess.check_output(["git", "rebase", "changed"])
        except subprocess.CalledProcessError as e:
            # Assuming that return code == 1 means that rebase conflicts
            if e.returncode != 1:
                raise e
            else:
                rebase_has_conflicts = True

        if rebase_has_conflicts:
            print(f"Navigate to {tmp_dir} to solve conflicts")

            while True:
                input("Press Enter to continue when conflicts have been resolved...")
                is_in_rebase_mode = True
                try:
                    subprocess.check_output(
                        '(test -d "$(git rev-parse --git-path rebase-merge)" || test -d "$(git rev-parse --git-path rebase-apply)")',
                        shell=True,
                    )
                except subprocess.CalledProcessError as e:
                    is_in_rebase_mode = False
                if is_in_rebase_mode:
                    print("Detected that you're still in rebase mode...")
                else:
                    break

        else:
            print(f"Successfully rebased without conflicts")


    print("Copying resolved changes back...")
    for folder in reversed(folders):
        if folder == changed_folder:
            print("Reached changed folder. Finish.")
            break

        shutil.copytree(
            tmp_dir,
            folder,
            dirs_exist_ok=True,
            ignore=shutil.ignore_patterns('.git')
        )
        with working_directory(tmp_dir):
            subprocess.check_output(["git", "checkout", "HEAD~1"])
        print(f"Updated folder: {folder}")
