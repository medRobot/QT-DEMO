#ifndef VTKCUSTOMCOMMANDS_H
#define VTKCUSTOMCOMMANDS_H

#include <QtGui/QMainWindow>
#include <vtkCommand.h>
#include <vtkImageViewer2.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleImage.h>
#include <vtkObject.h>

//�ο�vtkCommand.hд��
//�������Ķ������vtkCommand.cpp���򱨴�ԭ��û��Ū����2015.5.14
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

    vtkNextSliceCallbk()//���캯��
    {
        this->Interactor = 0;
    }

    void SetInteractorStyle(vtkInteractorStyleImage* style)
    {
        this->style = style;
    }

    void SetInteractor(vtkRenderWindowInteractor *interactor)//��д�����к���
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
        int max = originalViewer->GetSliceMax();//��������Ƭ��Ŀ�����ⷭҳ������Χ//63����0��ʼ
        int cur = originalViewer->GetSlice();//��õ�ǰ��Ƭ
        int num = resultViewer->GetSliceMax();//�жϽ����ͼ���Ƿ��Ѷ�������
        if (eventId == vtkCommand::MouseWheelForwardEvent && cur >= 0)//��ǰ
        {
            if(num == 0)//ֻ��originalViewer��������
            {
                if(cur == 0)
                    originalViewer->SetSlice(0);
                else
                    originalViewer->SetSlice(--cur);
                originalViewer->SetSliceOrientationToXY();
                Interactor->SetInteractorStyle((vtkInteractorObserver *)style);
                //viewer->Render();
                //Interactor->Start();//�Խ�����ⲻ͸��������ע�Ͳ��������л�slice�п��ܳ��ֶ�ջ���
            }
            else//originalViewer, resultViewer�������ݣ�Ҫ����������������
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

        if (eventId == vtkCommand::MouseWheelBackwardEvent &&  cur  <= max)//���
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

class vtkNextSliceCallbk2 : public vtkCommand//ֻ��һ�����ڷ�ҳ
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

    vtkNextSliceCallbk2()//���캯��
    {
        this->Interactor = 0;
    }

    void SetInteractorStyle(vtkInteractorStyleImage* style)
    {
        this->style = style;
    }

    void SetInteractor(vtkRenderWindowInteractor *interactor)//��д�����к���
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
        int max = viewer->GetSliceMax();//��������Ƭ��Ŀ�����ⷭҳ������Χ//63����0��ʼ
        int cur = viewer->GetSlice();//��õ�ǰ��Ƭ
        if (eventId == vtkCommand::MouseWheelForwardEvent && cur >= 0)//��ǰ
        {
            if(cur == 0)
                viewer->SetSlice(0);
            else
                viewer->SetSlice(--cur);
            viewer->SetSliceOrientationToXY();
            Interactor->SetInteractorStyle((vtkInteractorObserver *)style);
        } 

        if (eventId == vtkCommand::MouseWheelBackwardEvent &&  cur  <= max)//���
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
