#ifndef VTKCUSTOMCOMMANDS_H
#define VTKCUSTOMCOMMANDS_H

#include <QtGui/QMainWindow>
#include <vtkCommand.h>
#include <vtkImageViewer2.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleImage.h>
#include <vtkObject.h>

//参考vtkCommand.h写成
//如果将类的定义放在vtkCommand.cpp中则报错，原因没有弄懂—2015.5.14
class vtkNextSliceCallbk : public vtkCommand
{
private:
    vtkRenderWindowInteractor *Interactor;
    vtkImageViewer2* originalViewer;
    vtkImageViewer2* resultViewer;
    vtkInteractorStyleImage* style;

public:
    static vtkNextSliceCallbk *New()
    {
        return new vtkNextSliceCallbk;
    }

    vtkNextSliceCallbk()//构造函数
    {
        this->Interactor = 0;
    }

    void SetInteractorStyle(vtkInteractorStyleImage* style)
    {
        this->style = style;
    }

    void SetInteractor(vtkRenderWindowInteractor *interactor)//重写父类中函数
    {
        this->Interactor = interactor;
    }

    vtkRenderWindowInteractor *GetInteractor()
    {
        return this->Interactor;
    }

    void SetViewer(vtkImageViewer2* originalViewer, vtkImageViewer2* resultViewer)
    {
        this->originalViewer = originalViewer;
        this->resultViewer = resultViewer;
    }

    virtual void Execute(vtkObject *caller, unsigned long eventId, void *callData)
    {
        int max = originalViewer->GetSliceMax();//获得最大切片数目，避免翻页超出范围//63，从0开始
        int cur = originalViewer->GetSlice();//获得当前切片
        int num = resultViewer->GetSliceMax();//判断结果视图中是否已读入数据
        if (eventId == vtkCommand::MouseWheelForwardEvent && cur >= 0)//向前
        {
            if(num == 0)//只有originalViewer读入数据
            {
                if(cur == 0)
                    originalViewer->SetSlice(0);
                else
                    originalViewer->SetSlice(--cur);
                originalViewer->SetSliceOrientationToXY();
                Interactor->SetInteractorStyle((vtkInteractorObserver *)style);
                //viewer->Render();
                //Interactor->Start();//对交互理解不透彻，加入注释部分则在切换slice中可能出现堆栈溢出
            }
            else//originalViewer, resultViewer读入数据，要保持两个窗口联动
            {
                if(cur == 0)
                {
                    originalViewer->SetSlice(0);
                    resultViewer->SetSlice(0);
                }
                else
                {
                    originalViewer->SetSlice(--cur);
                    resultViewer->SetSlice(--cur);
                }
                originalViewer->SetSliceOrientationToXY();
                resultViewer->SetSliceOrientationToXY();
                Interactor->SetInteractorStyle((vtkInteractorObserver *)style);
            }
        } 

        if (eventId == vtkCommand::MouseWheelBackwardEvent &&  cur  <= max)//向后
        {
            if(num == 0)
            {
                if(cur == max)
                    originalViewer->SetSlice(max);
                else
                    originalViewer->SetSlice(++cur);
                originalViewer->SetSliceOrientationToXY();
                Interactor->SetInteractorStyle((vtkInteractorObserver *)style);
            }
            else
            {
                if(cur == max)
                {
                    originalViewer->SetSlice(max);
                    resultViewer->SetSlice(max);
                }
                else
                {
                    originalViewer->SetSlice(++cur);
                    resultViewer->SetSlice(++cur);
                }
                originalViewer->SetSliceOrientationToXY();
                resultViewer->SetSliceOrientationToXY();
                Interactor->SetInteractorStyle((vtkInteractorObserver *)style);
            }
        }
    }
};

class vtkNextSliceCallbk2 : public vtkCommand//只对一个窗口翻页
{
private:
    vtkRenderWindowInteractor *Interactor;
    vtkImageViewer2* viewer;
    vtkInteractorStyleImage* style;

public:
    static vtkNextSliceCallbk2 *New()
    {
        return new vtkNextSliceCallbk2;
    }

    vtkNextSliceCallbk2()//构造函数
    {
        this->Interactor = 0;
    }

    void SetInteractorStyle(vtkInteractorStyleImage* style)
    {
        this->style = style;
    }

    void SetInteractor(vtkRenderWindowInteractor *interactor)//重写父类中函数
    {
        this->Interactor = interactor;
    }

    vtkRenderWindowInteractor *GetInteractor()
    {
        return this->Interactor;
    }

    void SetViewer(vtkImageViewer2* viewer)
    {
        this->viewer = viewer;
    }

    virtual void Execute(vtkObject *caller, unsigned long eventId, void *callData)
    {
        int max = viewer->GetSliceMax();//获得最大切片数目，避免翻页超出范围//63，从0开始
        int cur = viewer->GetSlice();//获得当前切片
        if (eventId == vtkCommand::MouseWheelForwardEvent && cur >= 0)//向前
        {
            if(cur == 0)
                viewer->SetSlice(0);
            else
                viewer->SetSlice(--cur);
            viewer->SetSliceOrientationToXY();
            Interactor->SetInteractorStyle((vtkInteractorObserver *)style);
        } 

        if (eventId == vtkCommand::MouseWheelBackwardEvent &&  cur  <= max)//向后
        {
            if(cur == max)
                viewer->SetSlice(max);
            else
                viewer->SetSlice(++cur);
            viewer->SetSliceOrientationToXY();
            Interactor->SetInteractorStyle((vtkInteractorObserver *)style);
        }
    }
};

#endif // VTKCUSTOMCOMMANDS_H
