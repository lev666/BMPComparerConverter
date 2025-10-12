import asyncio
from enum import Enum
from pathlib import Path
import sys
from typing import Final

class NameTestsDir(Enum):
    SAME = "same"
    DIFF = "diff"

class Message_Err(Enum):
    MSG_OK = "OK"
    MSG_FAILED = "FAILED"
    MSG_PASSED = "PASSED"

TEST_IMAGE_1: Final[str] = "image1.bmp"
TEST_IMAGE_2: Final[str] = "image2.bmp"

IMAGES_SAME_MESSAGE: Final[str] = "Images are same"

RC_TOTALLY_DIFFERENT: Final[int] = 1
RC_DIFFERENT: Final[int] = 2


async def run_all_tests(comparer: Path, 
        test_data_dir: Path) -> int:
    same_tests_dir = Path.joinpath(test_data_dir, 
    NameTestsDir.SAME.value)
    diff_tests_dir = Path.joinpath(test_data_dir, 
    NameTestsDir.DIFF.value)
    f_tests = await asyncio.gather(
        run_same_tests(comparer, same_tests_dir),
        run_diff_tests(comparer, diff_tests_dir),
    )
    return sum(f_tests)


async def run_same_tests(comparer: Path, tests_dir: Path):
    f_tests = 0
    for test in sorted(map(lambda p: p.stem,
    tests_dir.iterdir())):
        image1 = Path.joinpath(tests_dir, test, 
        TEST_IMAGE_1)
        image2 = Path.joinpath(tests_dir, test,
        TEST_IMAGE_2)
        if not image1.exists() or not image2.exists():
            continue
        rc_curr, out_curr, err_curr = await run_test(
            comparer,
            (image1, image2),
        )
        if not validate_test(
            test,
            True,
            (0, rc_curr),
            (IMAGES_SAME_MESSAGE, out_curr),
            ("", err_curr),
        ):
            f_tests += 1
        return f_tests
        


async def run_diff_tests(comparer: Path, tests_dir: Path):
    f_tests = 0
    for test in sorted(map(lambda p: p.stem,
    tests_dir.iterdir())):
        image1 = Path.joinpath(tests_dir, test, 
        TEST_IMAGE_1)
        image2 = Path.joinpath(tests_dir, test,
        TEST_IMAGE_2)
        if not image1.exists() or not image2.exists():
            continue
        stderr_file = Path.joinpath(tests_dir, test,
        'output.txt')
        if stderr_file.exists():
            rc_correctly, out_correctly = RC_DIFFERENT, ""
            with open(stderr_file, "r") as errf: 
                err_correctly = errf.read().strip()
        else:
            rc_correctly, out_correctly, err_correctly = RC_TOTALLY_DIFFERENT, "", ""
        rc_curr, out_curr, err_curr = await run_test(
            comparer, 
            (image1, image2),
        )
        if not validate_test(
            test,
            False,
            (rc_correctly, rc_curr),
            (out_correctly, out_curr),
            (err_correctly, err_curr),
        ):
            f_tests += 1
        return f_tests



def validate_test(
        test_name: str,
        are_same: bool,
        rcs: tuple[int, int],
        outs: tuple[str | None, str],
        errs: tuple[str, str],
) -> bool:
    rc_correctly, rc_curr = rcs
    if rc_correctly != rc_curr:
        print(f"{Message_Err.MSG_FAILED.value} {test_name}: Return code {rc_correctly} != {rc_curr}")
        return False
    if are_same:
        out_correctly, out_curr = outs
        if out_correctly != out_curr:
            print(f"{Message_Err.MSG_FAILED.value} {test_name}: incorrect stdout")
            return False
    else:
        err_correctly, err_curr = errs
        if (rc_correctly == RC_DIFFERENT and err_correctly != err_curr) or not err_curr:
            print(f"{Message_Err.MSG_FAILED.value} {test_name}: incorrect stderr")
            return False
    print(f"{Message_Err.MSG_OK.value} {test_name}")
    return True

async def run_test(
        comparer: Path,
        imgs: tuple[Path, Path],
        ) -> tuple[int, str, str]:
    proc_exec = await asyncio.create_subprocess_exec(
        comparer, imgs[0], imgs[1],
        stdin=asyncio.subprocess.PIPE,
        stdout=asyncio.subprocess.PIPE,
        stderr=asyncio.subprocess.PIPE,
    )

    stdout, stderr = await proc_exec.communicate()
    rc = await proc_exec.wait()
    out_text = stdout.decode("ascii", "replace").strip()
    err_text = stderr.decode("ascii", "replace").strip()
    return rc, out_text, err_text


def main() -> int:
    comparer_exec = Path(sys.argv[1]).absolute()
    dataT = Path(sys.argv[2]).absolute()
    res_tests: int = asyncio.run(run_all_tests(
        comparer_exec, 
        dataT,))
    if res_tests > 0:
        print(f"{res_tests} tests {Message_Err.MSG_FAILED.value}\n")
        return 1
    print(f"All tests {Message_Err.MSG_PASSED.value}\n")
    return 0



if __name__ == "__main__":
    rc = main()
    sys.exit(rc)