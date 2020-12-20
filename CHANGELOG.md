- 1.0

  - Initial Version

- 1.1

  - Fixed crash upon seeking backwards (also causes crash in XMPlay on stopping)
  - Added version number to the plugin description.
  - Added 3 more interpolation options: B-spline, Hermite, and Optimal.

- 1.2

  - Fixed crash if Winamp tries to access a file that no longer exists, reported
    by Caitsith2.
  - Utilized the SSEQ's volume from the INFO section.
  - Minor optimizations.

- 1.3

  - Added more interpolation methods, cleanup of interpolation code, and stopped
    using slope to determine points outside the sample in an attempt to prevent
    clipping or popping.
  - Made it so certain changes in the configuration dialog will apply
    immediately after clicking OK.
  - Minor optimizations.

- 1.4

  - Removed the Optimal, Lagrange, and Hermite interpolations, added in
    2nd-order Osculating interpolation.
  - Changed handling of start/end of SWAV data to use the closest data point
    instead of only the current one.

- 1.5

  - Implemented circular interpolation buffer to fix interpolation, thanks to
    kode54 for the code.

- 1.6

  - Added Lanczos (Sinc) interpolation. Also modified Cosine interpolation to
    use a lookup table instead.

- 1.7

  - Fixed Lanczos interpolation a bit (thanks to kode54), also allowed the
    plugin to handle 32-bit samples.

- 1.7.1

  - Minor update to scale the phase offset, thanks to kode54.

- 1.8

  - Replaced Lanczos window for Sinc with the Hann window.
  - Added a ring buffer designed to work with SWAVs, replaces kode54's
    implementation (still have to give him big thanks for the original
    implementation).
  - Corrected clamping issue which caused some clipping-like effects.

- 1.8.1

  - Reverted Sinc interpolation back to the Lanczos window.

- 1.9

  - Removed the Cosine, B-Spline, and Osculating interpolations, added 4-point
    and 6-point Legrange interpolations in their place.
  - Updated zlib to v1.2.8.

- 1.9.1

  - Fixed volume issues that stemmed from how FeOS Sound System was handling
    volume.
  - Stopped utilizing SSEQ's volume from the INFO section, as it seemed like it
    wasn't used in the real thing.
  - Added a clone of DeSmuME's Sound View that only shows up in debug builds,
    was used to help me identify the above issue to fix it.

- 1.10

  - Implemented the random, variable, and conditional commands.
  - Fixed loop counter.

- 1.10.1

  - Moved track allocation into the SSEQ command handler on a suggestion from
    fincs when I had found a sequence that allocated tracks later than expected
    originally.

- 1.10.2

  - Correctly handle SSEQ volume as well as fix other volume issues.

- 1.10.3

  - Corrected attack rate calculation.
  - Corrected channel allocation causing sound cutoff, with lots of help from
    fincs.
  - Changed Sinc interpolation window to Nuttall 3-term on suggestion from
    kode54.

- 1.11

  - Utilize the PLAYER blocks in the SDAT if they exist.
  - Sinc modification from kode54, only the sinc pulse should scale, not the
    window.

- 1.11.1

  - Fixed unsigned underflow with a loop offset of 0 on an ADPCM SWAV.
