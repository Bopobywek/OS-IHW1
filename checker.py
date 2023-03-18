#!/usr/bin/python3

import argparse
import os
import subprocess

def transform_text(text):
    vowels = "aeiouy"
    new_text = ""

    for ch in text:
        if ch.isalpha() and ch.islower() and ch not in vowels:
            new_text += ch.upper()
        else:
            new_text += ch
    
    return new_text

parser = argparse.ArgumentParser(description='Checker for IHW-1')
parser.add_argument('-v', '--verbose', action='store_true')

args = parser.parse_args()

if __name__ == "__main__":
    print("Generating answers for tests...")
    for test in os.listdir(os.path.join("tests", "input")):
        with open(os.path.join("tests", "input", test)) as fin:
            data_in = fin.read()
        
        data_out = transform_text(data_in)

        with open(os.path.join("tests", "answers", f"{test}.a"), mode="w") as fout:
            fout.write(data_out)
    print("Complete\n")

    print("Looking for directories with scripts for testing...")
    for el in os.listdir("."):
        if os.path.isdir(el) and "run_tests.sh" in os.listdir(el):
            old_path = os.path.abspath(os.getcwd())
            print(f"Directory \"{el}\" contains script for run tests\nExecuting script...")
            os.chdir(os.path.join(os.curdir, el))
            subprocess.call("./run_tests.sh", stdout=subprocess.DEVNULL)
            print("Complete\n")

            print("Verifying answers...")
            for answer in os.listdir(os.path.join("tests", "output")):
                with open(os.path.join(old_path, "tests", "answers", answer)) as answer_file:
                    with open(os.path.join("tests", "output", answer)) as output:
                        is_correct = (answer_file.read() == output.read())
                        result = "correct" if is_correct else "wrong"
                        print(f"For test \"{answer.split()[0]}\" output is {result}")
            print("Verification complete\n")
            
            os.chdir(old_path)

