# SmartyLib_winc1500

Microchip/Atmel **ATWINC1500 host driver v19.5.4**, trimmed and ported for the SmartyPlugger
board (STM32U585, WINC1500 on SPI). Consumed as a git submodule at `_Lib/winc1500`.

## Provenance

- Driver sources: Microchip/Atmel ASF WINC1500 host driver 19.5.4 (BSD-3-Clause style licence, see
  the header of each source file).
- Forked from [`gd91/stm32_winc1500_station_example`](https://github.com/gd91/stm32_winc1500_station_example)
  (STM32F429 Nucleo demo, which bundles this driver in `winc1500/`). The demo application was
  removed and the driver moved to the repository root.
- `ARMmbed/sal-driver-atmel-winc1500` was checked and is **not** the origin: it is the older 19.4.4
  driver and differs in nearly every file.

## Local changes vs. upstream

All on top of the untouched upstream history, one commit each (`git log 4928a4e..`):

- `bsp/source/nm_bsp_stm32f429.c`, `bus_wrapper/source/nm_bus_wrapper_stm32f429.c`: ported from the
  STM32F4 HAL to the STM32U5 HAL and board-level `WIFI_*` macros (file names kept for a small diff).
- `config/conf_winc.h`: pins / SPI / EXTI mapped to `WIFI_*` macros provided by the host project.
- `driver/source/{m2m_wifi,nmdrv}.c`: `printf` -> `WIFI_LOG_I`.
- `root_cert/root_setup.c`: `X509/` include path case fix.
- `bus_wrapper`: SPI chip-select is released before returning on a transfer error.

`programmer/`, `root_cert/`, `driver/source/nmi2c.c` and `nmuart.c` are kept from upstream but are
excluded from the SmartyPlugger build.
