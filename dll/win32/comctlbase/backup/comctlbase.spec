@ stdcall MenuHelp(long long long long long long ptr)
@ stdcall ShowHideMenuCtl(long long ptr)
@ stdcall GetEffectiveClientRect(long long long)
@ stdcall DrawStatusTextA(long ptr str long)
@ stdcall CreateStatusWindowA(long str long long)
@ stdcall CreateToolbar(long long long long long long ptr long)
@ stdcall CreateMappedBitmap(long long long ptr long)
9 stdcall -noname DPA_LoadStream(ptr ptr ptr long)
10 stdcall -noname DPA_SaveStream(ptr ptr ptr long)
11 stdcall -noname DPA_Merge(ptr ptr long ptr ptr long)
@ stdcall CreatePropertySheetPage(ptr) CreatePropertySheetPageA
@ stdcall MakeDragList(long)
@ stdcall LBItemFromPt(long double long) #(long int64 long)
@ stdcall DrawInsert(long long long)
@ stdcall DrawShadowText(long wstr long ptr long long long long long)
@ stdcall CreateUpDownControl(long long long long long long long long long long long long)
@ stdcall InitCommonControls()
@ stdcall CreatePropertySheetPageA(ptr)
@ stdcall CreatePropertySheetPageW(ptr)
@ stdcall CreateStatusWindow(long str long long) CreateStatusWindowA
@ stdcall CreateStatusWindowW(long wstr long long)
@ stdcall CreateToolbarEx(long long long long long long ptr long long long long long long)
@ stdcall DestroyPropertySheetPage(long)
@ stdcall DllGetVersion(ptr)
@ stdcall DllInstall(long wstr)
@ stdcall DrawStatusText(long ptr ptr long) DrawStatusTextA
@ stdcall DrawStatusTextW(long ptr wstr long)
@ stdcall FlatSB_EnableScrollBar(long long long)
@ stdcall FlatSB_GetScrollInfo(long long ptr)
@ stdcall FlatSB_GetScrollPos(long long)
@ stdcall FlatSB_GetScrollProp(long long ptr)
@ stdcall FlatSB_GetScrollRange(long long ptr ptr)
@ stdcall FlatSB_SetScrollInfo(long long ptr long)
@ stdcall FlatSB_SetScrollPos(long long long long)
@ stdcall FlatSB_SetScrollProp(long long long long)
@ stdcall FlatSB_SetScrollRange(long long long long long)
@ stdcall FlatSB_ShowScrollBar(long long long)
@ stdcall GetMUILanguage()
@ stdcall ImageList_Add(ptr long long)
@ stdcall ImageList_AddIcon(ptr long)
@ stdcall ImageList_AddMasked(ptr long long)
@ stdcall ImageList_BeginDrag(ptr long long long)
@ stdcall ImageList_Copy(ptr long ptr long long)
@ stdcall ImageList_Create(long long long long long)
@ stdcall ImageList_Destroy(ptr)
@ stdcall ImageList_DragEnter(long long long)
@ stdcall ImageList_DragLeave(long)
@ stdcall ImageList_DragMove(long long)
@ stdcall ImageList_DragShowNolock(long)
@ stdcall ImageList_Draw(ptr long long long long long)
@ stdcall ImageList_DrawEx(ptr long long long long long long long long long)
@ stdcall ImageList_DrawIndirect(ptr)
@ stdcall ImageList_Duplicate(ptr)
@ stdcall ImageList_EndDrag()
@ stdcall ImageList_GetBkColor(ptr)
@ stdcall ImageList_GetDragImage(ptr ptr)
@ stdcall ImageList_GetFlags(ptr)
@ stdcall ImageList_GetIcon(ptr long long)
@ stdcall ImageList_GetIconSize(ptr ptr ptr)
@ stdcall ImageList_GetImageCount(ptr)
@ stdcall ImageList_GetImageInfo(ptr long ptr)
@ stdcall ImageList_GetImageRect(ptr long ptr)
@ stdcall ImageList_LoadImage(long str long long long long long) ImageList_LoadImageA
@ stdcall ImageList_LoadImageA(long str long long long long long)
@ stdcall ImageList_LoadImageW(long wstr long long long long long)
@ stdcall ImageList_Merge(ptr long ptr long long long)
@ stdcall ImageList_Read(ptr)
@ stdcall ImageList_ReadEx(long ptr long ptr)
@ stdcall ImageList_Remove(ptr long)
@ stdcall ImageList_Replace(ptr long long long)
@ stdcall ImageList_ReplaceIcon(ptr long long)
71 stdcall -noname Alloc(long)
72 stdcall -noname ReAlloc(ptr long)
73 stdcall -noname Free(ptr)
74 stdcall -noname GetSize(ptr)
@ stdcall ImageList_SetBkColor(ptr long)
@ stdcall ImageList_SetDragCursorImage(ptr long long long)
@ stdcall ImageList_SetFilter(ptr long long)
@ stdcall ImageList_SetFlags(ptr long)
@ stdcall ImageList_SetIconSize(ptr long long)
@ stdcall ImageList_SetImageCount(ptr long)
@ stdcall ImageList_SetOverlayImage(ptr long long)
@ stdcall ImageList_Write(ptr ptr)
@ stdcall ImageList_WriteEx(ptr long ptr)
@ stdcall InitCommonControlsEx(ptr)
@ stdcall InitMUILanguage(long)
@ stdcall InitializeFlatSB(long)
@ stdcall PropertySheet(ptr) PropertySheetA
@ stdcall PropertySheetA(ptr)
@ stdcall PropertySheetW(ptr)
@ stdcall RegisterClassNameW(wstr)
@ stdcall UninitializeFlatSB(long)
@ stdcall _TrackMouseEvent(ptr)
151 stdcall -noname CreateMRUListA(ptr)
152 stdcall -ordinal FreeMRUList(long)
153 stdcall -noname AddMRUStringA(long str)
154 stdcall -noname EnumMRUListA(long long ptr long)
155 stdcall -noname FindMRUStringA(long str ptr)
156 stdcall -noname DelMRUString(long long)
157 stdcall -noname CreateMRUListLazyA(ptr long long long)
163 stdcall -noname CreatePage(long ptr)
164 stdcall -noname CreateProxyPage(long long)
167 stdcall -noname AddMRUData(long ptr long)
169 stdcall -noname FindMRUData(long ptr long ptr)
233 stdcall -noname Str_GetPtrA(str str long)
234 stdcall -noname Str_SetPtrA(str str)
235 stdcall -noname Str_GetPtrW(wstr wstr long)
@ stdcall Str_SetPtrW(wstr wstr) ;maybe need ordinal
@ stdcall DSA_Create(long long);maybe need ordinal
@ stdcall DSA_Destroy(ptr) ;maybe need ordinal
322 stdcall -noname DSA_GetItem(ptr long long)
@ stdcall DSA_GetItemPtr(ptr long) ;maybe need ordinal
@ stdcall DSA_InsertItem(ptr long long) ;maybe need ordinal
325 stdcall -noname DSA_SetItem (ptr long long)
326 stdcall -noname DSA_DeleteItem(ptr long)
327 stdcall -ordinal DSA_DeleteAllItems(ptr)
@ stdcall DPA_Create(long)
@ stdcall DPA_Destroy(ptr)
330 stdcall -noname DPA_Grow(ptr long)
331 stdcall -noname DPA_Clone(ptr ptr)
332 stdcall -ordinal DPA_GetPtr(ptr long)
333 stdcall -noname DPA_GetPtrIndex(ptr ptr)
@ stdcall DPA_InsertPtr(ptr long ptr) ;maybe need ordinal
@ stdcall DPA_SetPtr(ptr long ptr) ;maybe need ordinal
@ stdcall DPA_DeletePtr(ptr long) ;maybe need ordinal
@ stdcall DPA_DeleteAllPtrs(ptr) ;maybe need ordinal
@ stdcall DPA_Sort(ptr ptr long) ;maybe need ordinal
@ stdcall DPA_Search(ptr ptr long ptr long long) ;maybe need ordinal
340 stdcall -noname DPA_CreateEx(long long)
341 stdcall -noname SendNotify(long long long ptr)
342 stdcall -noname SendNotifyEx(long long long ptr long)
350 stdcall -noname  StrChrA(str str)
351 stdcall -noname  StrRChrA(str str long)
352 stdcall -noname  StrCmpNA(str str long)
353 stdcall -noname  StrCmpNIA(str str long)
354 stdcall -noname  StrStrA(str str)
355 stdcall -noname  StrStrIA(str str)
356 stdcall -noname  StrCSpnA(str str)
357 stdcall -noname  StrToIntA(str)
358 stdcall -noname  StrChrW(wstr long)
359 stdcall -noname  StrRChrW(wstr wstr long)
360 stdcall -noname  StrCmpNW(wstr wstr long)
361 stdcall -noname  StrCmpNIW(wstr wstr long)
362 stdcall -noname  StrStrW(wstr wstr)
363 stdcall -noname  StrStrIW(wstr wstr)
364 stdcall -noname  StrCSpnW(wstr wstr)
365 stdcall -noname  StrToIntW(wstr)
366 stdcall -noname  StrChrIA(str long)
367 stdcall -noname  StrChrIW(wstr long)
368 stdcall -noname  StrRChrIA(str str long)
369 stdcall -noname  StrRChrIW(wstr wstr long)
372 stdcall -noname  StrRStrIA(str str str)
373 stdcall -noname  StrRStrIW(wstr wstr wstr)
374 stdcall -noname  StrCSpnIA(str str)
375 stdcall -noname  StrCSpnIW(wstr wstr)
376 stdcall -noname  IntlStrEqWorkerA(long str str long)
377 stdcall -noname  IntlStrEqWorkerW(long wstr wstr long)
382 stdcall -noname SmoothScrollWindow(ptr)
383 stdcall -noname DoReaderMode(ptr)
384 stdcall -noname SetPathWordBreakProc(ptr long)
@ stdcall DPA_EnumCallback(long long long) ;maybe need ordinal
@ stdcall DPA_DestroyCallback(ptr ptr long) ;maybe need ordinal
387 stdcall -noname DSA_EnumCallback(ptr ptr long)
@ stdcall DSA_DestroyCallback(ptr ptr long) ;maybe need ordinal
#389 CControl::v_OnNotify
390 stdcall -noname ImageList_SetColorTable(ptr long long ptr)
@ stdcall CreateMRUListW(ptr) ;maybe need ordinal
@ stdcall  AddMRUStringW(long wstr) ;maybe need ordinal
402 stdcall -noname FindMRUStringW(long wstr ptr)
403 stdcall -ordinal EnumMRUListW(long long ptr long)
404 stdcall -noname CreateMRUListLazyW(ptr long long long)
@ stdcall SetWindowSubclass(long ptr long long) ;maybe need ordinal
@ stdcall GetWindowSubclass(long ptr long ptr) ;maybe need ordinal
@ stdcall HIMAGELIST_QueryInterface(ptr long)
@ stdcall RemoveWindowSubclass(long ptr long) ;maybe need ordinal
@ stdcall DefSubclassProc(long long long long) ;maybe need ordinal
414 stdcall -noname MirrorIcon(ptr ptr)
415 stdcall -noname DrawTextWrap(long wstr long ptr long) user32.DrawTextW
416 stdcall -noname DrawTextExPrivWrap(long wstr long ptr long ptr) user32.DrawTextExW
417 stdcall -noname ExtTextOutWrap(long long long long ptr wstr long ptr) gdi32.ExtTextOutW
418 stdcall -noname GetCharWidthWrap(long long long long) gdi32.GetCharWidthW
419 stdcall -noname GetTextExtentPointWrap(long wstr long ptr) gdi32.GetTextExtentPointW
420 stdcall -noname GetTextExtentPoint32Wrap(long wstr long ptr) gdi32.GetTextExtentPoint32W
421 stdcall -noname TextOutWrap(long long long wstr long) gdi32.TextOutW