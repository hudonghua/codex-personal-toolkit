# Task Group: FSJ_666 remote display bitmap mapping
scope: Covers the ad-hoc note about `Src\App_lcd.c` / `Disp_Rock_Pic()` bitmap-to-button mapping for the FSJ_666 remote display. Use when adjusting icon positions, left/right top buttons, or receiver bit mapping in this checkout; treat it as project-specific and note-backed, not a general rule.
applies_to: cwd=FSJ_666 external project note; reuse_rule=checkout-sensitive, verify against the exact project before reusing outside this path family

## Task 1: Consolidated FSJ_666 display mapping note, completed

### rollout_summary_files

- extensions/ad_hoc/notes/2026-05-20-162031-ykq-display-mapping.md (cwd=FSJ_666 external project note, rollout_path=missing, updated_at=2026-05-20T16:20:31, thread_id=not_recorded, ad-hoc note [ad-hoc note])

### keywords

- FSJ_666, Src\App_lcd.c, Disp_Rock_Pic(), gDIBitV, NO1_BIT, NO2_BIT, NO3_BIT, 318.0.1, 318.1.0, 318.2.0, bit14, bit16~23

## Reusable knowledge

- `Disp_Rock_Pic()` is the main logic for the remote display icon mapping. When the top buttons need to move toward the middle, adjust the `vXX` start points and `vXX +=` spacing in that block first, not the bitmap asset table. [ad-hoc note]
- The note says the left/right vertical button mapping is already correct: left 4 buttons come from `gDIBitV bit16~19` -> receiver `318.2.0~3`, and right 4 buttons come from `gDIBitV bit20~23` -> receiver `318.2.4~7`. [ad-hoc note]
- The top 3 buttons are `NO1_BIT` / `NO2_BIT` / `NO3_BIT`, mapped to `318.4.0`, `318.4.1`, and `318.4.2`. [ad-hoc note]
- When the user says `direction reversed`, first try swapping the displayed index order `1 : 2` before assuming the bit positions are wrong. [ad-hoc note]
- The note’s confirmed mapping list includes: left top 1 -> `318.0.1`, left top 2 -> `318.0.0`, left top 3 -> `318.0.2`, right top 1 -> `318.1.0 down / 318.1.1 up`, right top 2 -> `318.1.4 down / 318.1.5 up`, right top 4 -> `318.1.3 down / 318.1.2 up`. [ad-hoc note]
- For the right top 3 button, the note says to use user-confirmed `gDIBitV>>8 bit6` (original `bit14`) and invert the direction as needed. [ad-hoc note]

## Failures and how to do differently

- Symptom: the icons look centered wrong or one row is offset. Cause: changing the bitmap table instead of the display block layout. Fix: edit `Disp_Rock_Pic()` `vXX` origins and spacing first. [ad-hoc note]
- Symptom: the screen looks mirrored or `direction reversed`. Cause: the displayed order is reversed, not necessarily the bit assignment. Fix: swap the index order `1 : 2` before rewriting the mapping. [ad-hoc note]
- Symptom: a mapping looks plausible in old comments but not in the actual device. Cause: the note says the middle two rows needed real receive-message backtracking. Fix: verify against the actual received report flow, not comments alone. [ad-hoc note]
