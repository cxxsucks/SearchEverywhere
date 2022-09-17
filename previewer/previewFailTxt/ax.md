# Preview Failed

This Office Document could not be previewed due to:

- Missing `LibreOffice` and
- Previewing via `MS Office` through `ActiveX` is not enabled  
    during compilation.

Installing `LibreOffice` would automatically enable previewing.

## I have `MS Office` installed

If you are **on Windows** and `MS Office` is installed,  
recompile the app with `CMake` flag `NEED_ACTIVEX_OFFICE_PREVIEW` enabled.  
Previewing office documents would work then.
