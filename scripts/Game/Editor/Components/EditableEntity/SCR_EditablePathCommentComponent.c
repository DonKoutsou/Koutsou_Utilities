[ComponentEditorProps(category: "GameScripted/Editor (Editables)", description: "", icon: "WBData/ComponentEditorProps/componentEditor.png")]
class SCR_EditablePathCommentComponentClass : SCR_EditableEntityComponentClass
{
}

/** @ingroup Editable_Entities
*/

/*!
*/
class SCR_EditablePathCommentComponent : SCR_EditableEntityComponent
{
	[Attribute(category: "Comment", defvalue: "1", uiwidget: UIWidgets.Slider, params: "0.5 2 0.1")]
	private float m_fSizeCoef;

	[Attribute(category: "Comment")]
	private bool m_bBold;

	[Attribute(category: "Comment")]
	private bool m_bItalic;

	
	private LightPost post
	//------------------------------------------------------------------------------------------------
	/*!
	Apply custom attributes on given text widget.
	\param textWidget Modified text
	*/
	void ApplyTo(TextWidget textWidget)
	{
		textWidget.SetText(GetDisplayName());
		textWidget.SetColor(post.GetColor());
		textWidget.SetBold(m_bBold);
		textWidget.SetItalic(m_bItalic);

		float size;
		textWidget.GetTextSize(size, size);
		textWidget.SetExactFontSize(size * m_fSizeCoef);
	}

	//------------------------------------------------------------------------------------------------
	/*!
	Apply custom attributes on given image widget.
	\param iconWidget Modified image
	*/
	void ApplyTo(ImageWidget iconWidget)
	{
		iconWidget.SetColor(post.GetColor());

		SCR_UIInfo info = GetInfo();
		if (info)
			info.SetIconTo(iconWidget);
	}

	//------------------------------------------------------------------------------------------------
	/*!
	Get comment color
	\return Color
	*/
	Color GetColor()
	{
		return post.GetColor();
	}

	//------------------------------------------------------------------------------------------------
	/*!
	Get font size coef
	\return Coef
	*/
	float GetSizeCoef()
	{
		return m_fSizeCoef;
	}

	//------------------------------------------------------------------------------------------------
	/*!
	Check if the test should be bold.
	\return True if bold
	*/
	bool IsBold()
	{
		return m_bBold;
	}

	//------------------------------------------------------------------------------------------------
	/*!
	Check if the test should be italic.
	\return True if italic
	*/
	bool IsItalic()
	{
		return m_bItalic;
	}
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		post = LightPost.Cast(owner.GetParent());
	}
#ifdef WORKBENCH
	private const float _WB_DRAW_DISTANCE = 1500 * 1500; //--- Squared value

	//------------------------------------------------------------------------------------------------
	override void _WB_AfterWorldUpdate(IEntity owner, float timeSlice)
	{
		vector cameraTransform[4];
		owner.GetWorld().GetCurrentCamera(cameraTransform);

		vector pos;
		GetPos(pos);
		//--- Draw only if near when not selected
		if (vector.DistanceSq(pos, cameraTransform[3]) > _WB_DRAW_DISTANCE)
			return;
		
		if (!SP_LightPostManager.GetInstane().m_benableDebug)
			return;
		
		if (!post)
			post = LightPost.Cast(owner.GetParent());

		int color;
		if (!post.GetColor())
		{
			color = Color.WHITE;
		}
		else
			color = post.GetColor().PackToInt();
		
		array <LightPost> ConnectedPost = {};
		SP_LightPostManager.GetConnectedPosts(post, ConnectedPost);
		if (!ConnectedPost.IsEmpty())
		{
			array <SP_BaseEn> ConnectingBases = {};			
			foreach(LightPost Dispost : ConnectedPost)
			{
				if (!Dispost)
					continue;
				Shape.CreateArrow(post.GetOrigin(), Dispost.GetOrigin(), 6, color, ShapeFlags.ONCE | ShapeFlags.NOZBUFFER);
			}
		}

	}
#endif
}
