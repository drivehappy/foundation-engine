/*!
	@file
	@author		Albert Semenov
	@date		11/2007
	@module
*/
#ifndef __MYGUI_WIDGET_H__
#define __MYGUI_WIDGET_H__

#include "MyGUI_Prerequest.h"
#include "MyGUI_Rtti.h"
#include "MyGUI_Any.h"
#include "MyGUI_ICroppedRectangle.h"
#include "MyGUI_ISubWidgetRect.h"
#include "MyGUI_ISubWidgetText.h"
#include "MyGUI_LayerItem.h"
#include "MyGUI_WidgetUserData.h"
#include "MyGUI_WidgetEvent.h"
#include "MyGUI_IWidgetCreator.h"
#include "MyGUI_WidgetSkinInfo.h"

namespace MyGUI
{

	class MYGUI_EXPORT Widget : public ICroppedRectangle, public LayerItem, public UserData, public WidgetEvent, public IWidgetCreator, public delegates::IDelegateUnlink
	{
		// для вызова закрытых деструкторов
		friend class IWidgetCreator;
		// для вызова закрытого конструктора
		friend class factory::BaseWidgetFactory<Widget>;

		MYGUI_RTTI_BASE_HEADER( Widget );

	public:

		// методы и шаблоны для создания виджета
		/** Create child widget
			@param _type widget type
			@param _skin widget skin
			@param _coord int coordinates of widget (_left, _top, _width, _height)
			@param _align widget align (possible values can be found in enum Align)
			@param _name if needed (you can use it for finding widget by name later)
		*/
		WidgetPtr createWidgetT(const std::string & _type, const std::string & _skin, const IntCoord& _coord, Align _align, const std::string & _name = "")
		{
			return baseCreateWidget(WidgetStyle::Child, _type, _skin, _coord, _align, "", _name);
		}
		/** See Widget::createWidgetT(const std::string & _type, const std::string & _skin, const IntCoord& _coord, Align _align, const std::string & _name = "") */
		WidgetPtr createWidgetT(const std::string & _type, const std::string & _skin, int _left, int _top, int _width, int _height, Align _align, const std::string & _name = "")
		{
			return createWidgetT(_type, _skin, IntCoord(_left, _top, _width, _height), _align, _name);
		}

		/** Create widget using coordinates relative to parent. see Widget::createWidgetT(const std::string & _type, const std::string & _skin, const IntCoord& _coord, Align _align, const std::string & _name = "") */
		WidgetPtr createWidgetRealT(const std::string & _type, const std::string & _skin, const FloatCoord& _coord, Align _align, const std::string & _name = "");
		/** Create widget using coordinates relative to parent. see Widget::createWidgetT(const std::string & _type, const std::string & _skin, const IntCoord& _coord, Align _align, const std::string & _name = "") */
		WidgetPtr createWidgetRealT(const std::string & _type, const std::string & _skin, float _left, float _top, float _width, float _height, Align _align, const std::string & _name = "")
		{
			return createWidgetRealT(_type, _skin, FloatCoord(_left, _top, _width, _height), _align, _name);
		}

		// templates for creating widgets by type
		/** Same as Widget::createWidgetT but return T pointer instead of WidgetPtr */
		template <typename T>
		T* createWidget(const std::string & _skin, const IntCoord& _coord, Align _align, const std::string & _name = "")
		{
			return static_cast<T*>(createWidgetT(T::getClassTypeName(), _skin, _coord, _align, _name));
		}
		/** Same as Widget::createWidgetT but return T pointer instead of WidgetPtr */
		template <typename T>
		T* createWidget(const std::string & _skin, int _left, int _top, int _width, int _height, Align _align, const std::string & _name = "")
		{
			return static_cast<T*>(createWidgetT(T::getClassTypeName(), _skin, IntCoord(_left, _top, _width, _height), _align, _name));
		}
		/** Same as Widget::createWidgetRealT but return T* instead of WidgetPtr */
		template <typename T>
		T* createWidgetReal(const std::string & _skin, const FloatCoord& _coord, Align _align, const std::string & _name = "")
		{
			return static_cast<T*>(createWidgetRealT(T::getClassTypeName(), _skin, _coord, _align, _name));
		}
		/** Same as Widget::createWidgetRealT but return T* instead of WidgetPtr */
		template <typename T>
		T* createWidgetReal(const std::string & _skin, float _left, float _top, float _width, float _height, Align _align, const std::string & _name = "")
		{
			return static_cast<T*>(createWidgetRealT(T::getClassTypeName(), _skin, _left, _top, _width, _height, _align, _name));
		}

		/** Create child widget
			@param _style Child, Popup or Overlapped widget style
			@param _type widget type
			@param _skin widget skin
			@param _coord int coordinates of widget (_left, _top, _width, _height)
			@param _align widget align (possible values can be found in enum Align)
			@param _name if needed (you can use it for finding widget by name later)
		*/
		WidgetPtr createWidgetT(WidgetStyle _style, const std::string & _type, const std::string & _skin, const IntCoord& _coord, Align _align, const std::string & _layer = "", const std::string & _name = "")
		{
			return baseCreateWidget(_style, _type, _skin, _coord, _align, _layer, _name);
		}
		/** Same as Widget::createWidgetT but return T* instead of WidgetPtr */
		template <typename T>
		T* createWidget(WidgetStyle _style, const std::string & _skin, const IntCoord& _coord, Align _align, const std::string & _layer = "", const std::string & _name = "")
		{
			return static_cast<T*>(createWidgetT(_style, T::getClassTypeName(), _skin, _coord, _align, _layer, _name));
		}

		//! Get name of widget
		const std::string & getName() { return mName; }


		/** Set widget position (position of left top corner)*/
		virtual void setPosition(const IntPoint& _pos);
		/** Set widget size */
		virtual void setSize(const IntSize& _size);
		/** Set widget position and size*/
		virtual void setCoord(const IntCoord& _coord);

		/** See Widget::setPosition(const IntPoint& _pos) */
		void setPosition(int _left, int _top) { setPosition(IntPoint(_left, _top)); }
		/** See Widget::setSize(const IntSize& _size) */
		void setSize(int _width, int _height) { setSize(IntSize(_width, _height)); }
		/** See Widget::setCoord(const IntCoord& _coord) */
		void setCoord(int _left, int _top, int _width, int _height) { setCoord(IntCoord(_left, _top, _width, _height)); }

		/** Set widget position (position of left top corner)*/
		void setRealPosition(const FloatPoint & _point);
		/** Set widget size */
		void setRealSize(const FloatSize & _size);
		/** Set widget position and size*/
		void setRealCoord(const FloatCoord & _coord);

		/** See Widget::setRealPosition(const FloatPoint& _point) */
		void setRealPosition(float _left, float _top) { setRealPosition(FloatPoint(_left, _top)); }
		/** See Widget::setRealSize(const FloatSize& _size) */
		void setRealSize(float _width, float _height) { setRealSize(FloatSize(_width, _height)); }
		/** See Widget::setRealPosition(const FloatCoord& _coord) */
		void setRealCoord(float _left, float _top, float _width, float _height) { setRealCoord(FloatCoord(_left, _top, _width, _height)); }

		/** Hide or show widget */
		virtual void setVisible(bool _visible);

		/** Set widget caption */
		virtual void setCaption(const Ogre::UTFString & _caption);
		/** Get widget caption */
		virtual const Ogre::UTFString & getCaption();

		/** Set widget text font */
		virtual void setFontName(const std::string & _font);
		/** Get widget text font name */
		virtual const std::string & getFontName();

		/** Set widget text font height */
		virtual void setFontHeight(uint _height);
		/** Get widget text font height */
		virtual uint getFontHeight();

		/** Set widget text align */
		virtual void setTextAlign(Align _align);
		/** Get widget text align */
		virtual Align getTextAlign();

		/** Set widget text colour */
		virtual void setTextColour(const Colour& _colour);
		/** Get widget text colour */
		virtual const Colour& getTextColour();

		/** Set widget opacity */
		void setAlpha(float _alpha);
		/** Get widget opacity */
		float getAlpha() { return mAlpha; }

		/** Get inherits alpha mode flag */
		bool isInheritsAlpha() { return mInheritsAlpha; }
		/** Enable or disable inherits alpha mode */
		void setInheritsAlpha(bool _inherits);

		/** Set widget's state */
		bool setState(const std::string & _state);

		// являемся ли мы рутовым виджетом
		/** Is this widget is root widget (root == without parents) */
		bool isRootWidget() { return nullptr == mCroppedParent; }

		/** Get parent widget or nullptr if no parent */
		WidgetPtr getParent() { return mParent; }

		/** Get child widgets Enumerator */
		EnumeratorWidgetPtr getEnumerator()
		{
			if (mWidgetClient) return mWidgetClient->getEnumerator();
			return Enumerator<VectorWidgetPtr>(mWidgetChild.begin(), mWidgetChild.end());
		}

		/** Find widget by name (search recursively through all childs starting from this widget) */
		WidgetPtr findWidget(const std::string & _name);

		/** Is need key focus */
		bool isNeedKeyFocus() { return mNeedKeyFocus; }
		/** Set need key focus flag */
		void setNeedKeyFocus(bool _need) { mNeedKeyFocus = _need; }
		/** Is need mouse focus */
		bool isNeedMouseFocus() { return mNeedMouseFocus; }
		/** Set need mouse focus flag */
		void setNeedMouseFocus(bool _need) { mNeedMouseFocus = _need; }

		/** Set inherits mode flag */
		void setInheritsPick(bool _inherits) { mInheritsPick = _inherits; }
		/** Get inherits mode flag */
		bool isInheritsPick() { return mInheritsPick; }

		/** Set picking mask for widget */
		void setMaskPick(const std::string & _filename);

		/** Is widget enabled */
		bool isEnabled() { return mEnabled; }
		/** Enable or disable widget */
		virtual void setEnabled(bool _enabled);

		/** Enable or disable widget without changing widget's state */
		void setEnabledSilent(bool _enabled) { mEnabled = _enabled; }

		/** Get mouse pointer name for this widget */
		const std::string& getPointer()
		{
			if (false == mEnabled) {
				static std::string empty;
				return empty;
			}
			return mPointer;
		}

		/** Set mouse pointer for this widget */
		void setPointer(const std::string& _pointer) { mPointer = _pointer; }

		/** Get widget's layer, return "" if widget is not root widget (root == without parents) */
		const std::string& getLayerName();

		/** Get rect where child widgets placed */
		IntCoord getClientCoord();

		/** Get clien area widget */
		WidgetPtr getClientWidget() { return mWidgetClient; }

		/** Get text sub widget or nullptr if no text sub widget */
		ISubWidgetText * getSubWidgetText() { return mText; }
		/** Get sub widget of first texture or nullptr if no sub widget with texture */
		ISubWidgetRect * getSubWidgetMain() { return mMainSkin; }

		/** Get need tool tip mode flag */
		bool getNeedToolTip() { return mNeedToolTip; }
		/** Set need tool tip mode flag. Enable this if you need tool tip events for widget */
		void setNeedToolTip(bool _need);

		/** Get tool tip enabled flag */
		bool getEnableToolTip() { return mEnableToolTip; }
		/** Enable or disable tooltip event */
		void setEnableToolTip(bool _enable);

		/** Detach widget from widgets hierarchy */
		void detachFromWidget();
		/** Attach widget to parent */
		void attachToWidget(WidgetPtr _parent);

		/** Change widget skin */
		void changeWidgetSkin(const std::string& _skinname);

		/** Get widget style */
		WidgetStyle getWidgetStyle() { return mWidgetStyle; }

		/** Set widget style.
			@note When choosing WidgetStyle::Popup style you also need attach widget to layer
			see LayerManager::attachToLayerKeeper
		*/
		void setWidgetStyle(WidgetStyle _style);


	/*internal:*/
		// метод для запроса номера айтема и контейнера
		virtual void _getContainer(WidgetPtr & _container, size_t & _index);

		// дает приоритет виджету при пиккинге
		void _forcePeek(WidgetPtr _widget);

		void _setUVSet(const FloatRect& _rect);

		virtual void _setTextureName(const std::string& _texture);
		virtual const std::string& _getTextureName();

		// наследуемся он LayerInfo
		virtual LayerItem * _findLayerItem(int _left, int _top);
		virtual void _attachToLayerItemKeeper(LayerItemKeeper * _item, bool _deep = false);
		virtual void _detachFromLayerItemKeeper(bool _deep = false);

		IWidgetCreator * _getIWidgetCreator() { return mIWidgetCreator; }


	/*obsolete:*/
#ifndef MYGUI_DONT_USE_OBSOLETE

		MYGUI_OBSOLETE("use : void Widget::setCoord(const IntCoord& _coord)")
		void setPosition(const IntCoord & _coord) { setCoord(_coord); }
		MYGUI_OBSOLETE("use : void Widget::setCoord(int _left, int _top, int _width, int _height)")
		void setPosition(int _left, int _top, int _width, int _height) { setCoord(_left, _top, _width, _height); }

		MYGUI_OBSOLETE("use : void Widget::setEnableToolTip")
		void enableToolTip(bool _enable) { setEnableToolTip(_enable); }

		MYGUI_OBSOLETE("use : void setInheritsPick(bool _inherits)")
		void setInheritsPeek(bool _inherits) { setInheritsPick(_inherits); }
		MYGUI_OBSOLETE("use : bool isInheritsPick()")
		bool isInheritsPeek() { return isInheritsPick(); }

		MYGUI_OBSOLETE("use : void setMaskPick(const std::string & _filename)")
		void setMaskPeek(const std::string & _filename) { setMaskPick(_filename); }

		MYGUI_OBSOLETE("use : void Widget::setTextColour(const Colour& _colour)")
		void setColour(const Colour& _colour) { setTextColour(_colour); }
		MYGUI_OBSOLETE("use : const Colour& Widget::getTextColour()")
		const Colour& getColour() { return getTextColour(); }

		MYGUI_OBSOLETE("use : const IntCoord& ISubWidgetText::getCoord()")
		IntCoord getTextCoord();
		MYGUI_OBSOLETE("use : IntSize ISubWidgetText::getTextSize()")
		IntSize getTextSize();

#endif // MYGUI_DONT_USE_OBSOLETE

	protected:
		// все создание только через фабрику
		Widget(WidgetStyle _style, const IntCoord& _coord, Align _align, const WidgetSkinInfoPtr _info, WidgetPtr _parent, ICroppedRectangle * _croppedParent, IWidgetCreator * _creator, const std::string & _name);
		virtual ~Widget();

		virtual void baseChangeWidgetSkin(WidgetSkinInfoPtr _info);

		void _updateView(); // обновления себя и детей

		void _setAlign(const IntSize& _size, bool _update);
		void _setAlign(const IntCoord& _coord, bool _update);

		// создает виджет
		virtual WidgetPtr baseCreateWidget(WidgetStyle _style, const std::string & _type, const std::string & _skin, const IntCoord& _coord, Align _align, const std::string & _layer, const std::string & _name);

		// удяляет неудачника
		virtual void _destroyChildWidget(WidgetPtr _widget);

		// удаляет всех детей
		virtual void _destroyAllChildWidget();

		// запрашиваем у конейтера айтем по позиции мыши
		virtual size_t _getContainerIndex(const IntPoint & _point) { return ITEM_NONE; }

		// сброс всех данных контейнера, тултипы и все остальное
		virtual void _resetContainer(bool _update);

		// аттачит с помощью _attachToLayerItemKeeper в зависимости от стиля
		void _attachToLayerItemKeeperByStyle(LayerItemKeeper * _item, bool _deep);

		// детачит с помощью _detachFromLayerItemKeeper в зависимости от стиля
		void _detachFromLayerItemKeeperByStyle(bool _deep);

	private:

		void frameEntered(float _frame);

		void initialiseWidgetSkin(WidgetSkinInfoPtr _info, const IntSize& _size);
		void shutdownWidgetSkin(bool _deep = false);

		void _updateAlpha();
		void _updateAbsolutePoint();

		// для внутреннего использования
		void _setInheritsVisible(bool _visible);
		bool _isInheritsVisible() { return mInheritsVisible; }

		// показывает скрывает все сабскины
		void _setSubSkinVisible(bool _visible);

		float _getRealAlpha() { return mRealAlpha; }

		// добавляет в список виджет
		virtual void _linkChildWidget(WidgetPtr _widget);
		// удаляет из списка
		virtual void _unlinkChildWidget(WidgetPtr _widget);

	protected:
		// список всех стейтов
		MapWidgetStateInfo mStateInfo;
		// информация о маске для пикинга
		MaskPickInfo const * mMaskPickInfo;
		MaskPickInfo mOwnMaskPickInfo;

		// вектор всех детей виджетов
		VectorWidgetPtr mWidgetChild;
		// вектор детей скина
		VectorWidgetPtr mWidgetChildSkin;
		// вектор всех детей сабскинов
		VectorSubWidget mSubSkinChild;

		// указатель на окно текста
		ISubWidgetText * mText;
		// указатель на первый не текстовой сабскин
		ISubWidgetRect * mMainSkin;

		// доступен ли на виджет
		bool mEnabled;
		// скрыты ли все сабскины при выходе за границу
		bool mSubSkinsVisible;
		// для иерархического скрытия
		bool mInheritsVisible;
		// прозрачность и флаг наследования альфы нашего оверлея
		float mAlpha;
		float mRealAlpha;
		bool mInheritsAlpha;
		// имя виджета
		std::string mName;
		// курсор который будет показан при наведении
		std::string mPointer;
		std::string mTexture;

		// наш отец в иерархии виджетов
		WidgetPtr mParent;

		// это тот кто нас создал, и кто нас будет удалять
		IWidgetCreator * mIWidgetCreator;

		// нужен ли виджету ввод с клавы
		bool mNeedKeyFocus;
		// нужен ли виджету фокус мыши
		bool mNeedMouseFocus;
		bool mInheritsPick;

		// клиентская зона окна
		// если виджет имеет пользовательские окна не в себе
		// то обязательно проинициализировать Client
		WidgetPtr mWidgetClient;

		bool mNeedToolTip;
		bool mEnableToolTip;
		bool mToolTipVisible;
		float mToolTipCurrentTime;
		IntPoint mToolTipOldPoint;
		size_t mToolTipOldIndex;
		IntPoint m_oldMousePoint;
		size_t mOldToolTipIndex;

		// поведение виджета, перекрывающийся дочерний или всплывающий
		WidgetStyle mWidgetStyle;

	};

} // namespace MyGUI

#endif // __MYGUI_WIDGET_H__