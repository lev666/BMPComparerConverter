import sys
import asyncio
from pathlib import Path
from enum import Enum
from typing import Final


class NameTestsDir(Enum):
    OK = "ok"
    N_OK = "not_ok"
    TWICE = "twice"


class Message_Err(Enum):
    MSG_OK = "OK"
    MSG_FAILED = "FAILED"
    MSG_PASSED = "PASSED"


INPUT_IMAGE: Final[str] = "image.bmp"
OUTPUT_IMAGE: Final[str] = "image_neg.bmp"
REFERENCE_IMAGE: Final[str] = "image_neg_reference.bmp"
OUTPUT_TWICE_IMAGE: Final[str] = "image_neg_twice.bmp"

IMAGES_SAME_MESSAGE: Final[str] = "Images are same"


async def start_all_tests(
        testDataDir: Path,
        converter: Path,
        comparer: Path,
        ) -> int:
    ok_dir = Path.joinpath(testDataDir, NameTestsDir.OK.value)
    n_ok_dir = Path.joinpath(testDataDir, NameTestsDir.N_OK.value)
    twice_dir = Path.joinpath(testDataDir, NameTestsDir.TWICE.value)
    fail_tests = await asyncio.gather(
        run_ok_tests(ok_dir, converter, comparer),
        run_n_ok_tests(n_ok_dir, converter),
        run_twice_tests(twice_dir, converter, comparer),
    )
    return sum(fail_tests)


async def run_ok_tests(
        ok_dir: Path, 
        converter: Path, 
        comparer: Path,
        ) -> int:
    f_tests = 0
    for test in sorted(map(lambda p: p.stem, ok_dir.iterdir())):
        image_in = Path.joinpath(ok_dir, test, INPUT_IMAGE)
        if not image_in.exists():
            continue
        image_out = Path.joinpath(ok_dir, test, OUTPUT_IMAGE)
        image_our_ref = Path.joinpath(ok_dir, test, REFERENCE_IMAGE)
        rc_curr, out_curr, err_curr = await run_test(
            converter, 
            (image_in, image_out)
            )
        if not await validate_ok_test(
            test,
            comparer,
            (image_out, image_our_ref),
            (0, rc_curr),
            ("", out_curr),
            ("", err_curr),
        ):
            f_tests += 1
        if image_out.exists():
            image_out.unlink()
    return f_tests


async def run_n_ok_tests(
        n_ok_dir: Path, 
        converter: Path, 
        ) -> int:
    f_tests = 0
    for test in sorted(map(lambda p: p.stem, n_ok_dir.iterdir())):
        image_in = Path.joinpath(n_ok_dir, test, INPUT_IMAGE)
        if not image_in.exists:
            continue
        image_out = Path.joinpath(n_ok_dir, test, OUTPUT_IMAGE)
        rc_curr, out_curr, err_curr = await run_test(
            converter, (image_in, image_out)
        )
        if not validate_not_ok_test(
            test,
            image_out,
            (1, rc_curr),
            ("", out_curr),
            ("", err_curr),
        ):
            f_tests += 1
        if image_out.exists():
            image_out.unlink()
    return f_tests


async def run_twice_tests(
        twice_dir: Path, 
        converter: Path, 
        comparer: Path,
        ) -> int:
    f_tests = 0
    for test in sorted(map(lambda p: p.stem, twice_dir.iterdir())):
        image_in = Path.joinpath(twice_dir, test, INPUT_IMAGE)
        if not image_in.exists:
            continue
        image_out = Path.joinpath(twice_dir, test, OUTPUT_IMAGE)
        image_out_twice = Path.joinpath(twice_dir, test, OUTPUT_TWICE_IMAGE)
        rc_curr, out_curr, err_curr = await run_test(
            converter, (image_in, image_out)
        )
        await run_test(
            converter, (image_out, image_out_twice)
        )
        if not await validate_ok_test(
            test,
            comparer,
            (image_in, image_out_twice),
            (0, rc_curr),
            ("", out_curr),
            ("", err_curr),
        ):
            f_tests += 1
        if image_out.exists():
            image_out.unlink()
        if image_out_twice.exists():
            image_out_twice.unlink()
        return f_tests
        

async def validate_ok_test(
        test_name: str,
        comparer: Path,
        imgs: tuple[Path, Path],
        rcs: tuple[int, int],
        outs: tuple[str, str],
        errs: tuple[str, str],
) -> bool:
    rc_correctly, rc_curr = rcs
    if rc_correctly != rc_curr:
        print(f"{Message_Err.MSG_FAILED.value} {test_name}: Return code {rc_correctly} != {rc_curr}")
        return False
    out_correctly, out_curr = outs
    if out_correctly != out_curr:
        print(f"{Message_Err.MSG_FAILED.value} {test_name}: incorrect stdout")
        return False
    err_correctly, err_curr = errs
    if err_correctly != err_curr:
        print(f"{Message_Err.MSG_FAILED.value} {test_name}: incorrect stderr")
        return False
    
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
    if rc != rc_correctly or out_text != IMAGES_SAME_MESSAGE or err_text != err_correctly:
        print(f"{Message_Err.MSG_FAILED.value} {test_name}: result image is incorrect")
        return False
    print(f"{Message_Err.MSG_OK.value} {test_name}")
    return True


def validate_not_ok_test(
        test_name: str,
        img: Path,
        rcs: tuple[int, int],
        outs: tuple[str | None, str],
        errs: tuple[str | None, str],
) -> bool:
    rc_correctly, rc_curr = rcs
    if rc_correctly != rc_curr:
        print(f"{Message_Err.MSG_FAILED.value} {test_name}: Return code {rc_correctly} != {rc_curr}")
        return False
    out_correctly, out_curr = outs
    if out_correctly != out_curr:
        print(f"{Message_Err.MSG_FAILED.value} {test_name}: incorrect stdout")
        return False
    
    if not errs[1]:
        print(f"{Message_Err.MSG_FAILED.value} {test_name}: incorrect stderr")
        return False
    if img.exists():
        print(f"{Message_Err.MSG_FAILED.value} {test_name}: output file created in incorect scenario")
        return False
    print(f"{Message_Err.MSG_OK.value} {test_name}")
    return True


async def run_test(
        converter: Path, 
        images: tuple[Path, Path],
        ) -> tuple[int, str, str]:
    img_in, img_out = images
    if img_out.exists():
        img_out.unlink()
    proc_exec = await asyncio.create_subprocess_exec(
        converter, img_in, img_out,
        stdin=asyncio.subprocess.PIPE,
        stdout=asyncio.subprocess.PIPE,
        stderr=asyncio.subprocess.PIPE
    )
    stdout, stderr = await proc_exec.communicate()
    rc = await proc_exec.wait()
    out_text = stdout.decode("ascii", "replace").strip()
    err_text = stderr.decode("ascii", "replace").strip()
    return rc, out_text, err_text


def main() -> int:
    dataT_dir = Path(sys.argv[2]).absolute()
    converter_exec = Path(sys.argv[1]).absolute()
    comparer_exec = Path(sys.argv[3]).absolute()
    res_tests: int = asyncio.run(start_all_tests(
        dataT_dir, 
        converter_exec, 
        comparer_exec))
    if res_tests > 0:
        print(f"{res_tests} tests {Message_Err.MSG_FAILED.value}\n")
        return 1
    print(f"All tests {Message_Err.MSG_PASSED.value}\n")
    return 0


if __name__ == "__main__":
    rc = main()
    sys.exit(rc)
