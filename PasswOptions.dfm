�
 TPASSWOPTIONSDLG 0e  TPF0TPasswOptionsDlgPasswOptionsDlgLeft� Top� HorzScrollBar.VisibleVertScrollBar.VisibleAnchors BorderIconsbiSystemMenu
biMaximize CaptionAdvanced Password OptionsClientHeight�ClientWidth�Color	clBtnFaceConstraints.MinHeight�Constraints.MinWidth�Font.CharsetDEFAULT_CHARSET
Font.ColorclWindowTextFont.Height�	Font.NameTahoma
Font.Style OldCreateOrder
OnActivateFormActivateOnShowFormShow
DesignSize�� PixelsPerInch`
TextHeight TLabelInfoLblLeftTop� Width�Height9AnchorsakLeftakRightakBottom AutoSizeWordWrap	  TLabelAmbigCharsLblLeftTop� Width� HeightAnchorsakLeftakBottom CaptionRedefine ambiguous characters:  TLabelSpecialSymLblLeftTop� Width{HeightAnchorsakLeftakBottom CaptionRedefine special symbols:  TLabelMaxWordLenLblLeftTop(Width5HeightAnchorsakLeftakBottom CaptionAMaximum length of words in word lists (except default word list):  TLabelTrigramFileLblLeftTopXWidth5HeightAnchorsakLeftakBottom Caption?Trigram file for generating phonetic (pronounceable) passwords:  TBevel	SeparatorLeftTop� Width�Height	AnchorsakLeftakRightakBottom Shape	bsTopLine  TCheckListBoxPasswOptionsListLeftTopWidth�HeightqAnchorsakLeftakTopakRightakBottom Flat
ItemHeightItems.StringsExclude ambiguous characters/First character must not be a lower-case letterDon't separate words by a space6Don't separate words and characters by a '-' character4Reverse default order of character/word combinations,Include at least one upper-case letter (A-Z),Include at least one lower-case letter (a-z) Include at least one digit (0-9).Include at least one special symbol (!"#$%...)1Only include characters from custom character set6Exclude repeating consecutive characters (e.g. aa, 11)+Exclude duplicate entries in password lists-Convert all words in word lists to lower-case#Each character must occur only onceEach word must occur only once 	PopupMenuListMenuStylelbOwnerDrawFixedTabOrder   TUpDownMaxWordLenSpinBtnLeft9Top8WidthHeightAnchorsakLeftakBottom 	AssociateMaxWordLenBoxMinPositionTabOrder	Thousands  TButtonOKBtnLeftTop�WidthKHeightAnchorsakRightakBottom CaptionOKDefault	ModalResultTabOrder  TButton	CancelBtnLeftXTop�WidthKHeightAnchorsakRightakBottom Cancel	CaptionCancelModalResultTabOrder  TButton	BrowseBtnLeft�TopfWidth!HeightHintBrowseAnchorsakRightakBottom Caption...ParentShowHintShowHint	TabOrderOnClickBrowseBtnClick  TEditAmbigCharsBoxLeftTop� Width�HeightAnchorsakLeftakRightakBottom TabOrder  TEditSpecialSymBoxLeftTopWidth�HeightAnchorsakLeftakRightakBottom TabOrder  TEditMaxWordLenBoxLeftTop8Width1HeightAnchorsakLeftakBottom TabOrderText1OnExitMaxWordLenBoxExit  TEditTrigramFileBoxLeftTophWidthqHeightAnchorsakLeftakRightakBottom TabOrder  
TPopupMenuListMenuLeftTop� 	TMenuItemListMenu_SelectAllCaption
Select AllOnClickListMenu_SelectAllClick  	TMenuItemListMenu_DeselectAllCaptionDeselect AllOnClickListMenu_DeselectAllClick  	TMenuItemListMenu_N1Caption-  	TMenuItemListMenu_InvertSelectionCaptionInvert SelectionOnClickListMenu_InvertSelectionClick    