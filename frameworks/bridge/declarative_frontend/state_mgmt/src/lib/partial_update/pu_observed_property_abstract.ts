/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

abstract class ObservedPropertyAbstractPU<T> extends ObservedPropertyAbstract<T> {

  private markDependentElementsIsPending: boolean = false;
  private dependentElementIds_: Set<number> = new Set<number>();

  constructor(subscribingView: IPropertySubscriber, viewName: PropertyInfo) {
    super(subscribingView, viewName);
  }

  protected notifyHasChanged(newValue: T) {
    super.notifyHasChanged(newValue);

    // for properties owned by a View"
    // markDependentElementsDirty needs to be executed
    this.markDependentElementsIsPending = true;
  }

  protected notifyPropertyRead() {
    super.notifyPropertyRead();
    this.recordDependentUpdate();
  }



  // FIXME unification: method needed, what callses to create here?

  /**
   * factory function for concrete 'object' or 'simple' ObservedProperty object
   * depending if value is Class object
   * or simple type (boolean | number | string)
   * @param value
   * @param owningView
   * @param thisPropertyName
   * @returns either
   */
  static CreateObservedObject<C>(value: C, owningView: IPropertySubscriber, thisPropertyName: PropertyInfo)
    : ObservedPropertyAbstract<C> {
    return (typeof value === "object") ?
      new ObservedPropertyObject(value, owningView, thisPropertyName)
      : new ObservedPropertySimple(value, owningView, thisPropertyName);
  }


  /**
   * during 'get' access recording take note of the created component and its elmtId
   * and add this component to the list of components who are dependent on this property
   */
  protected recordDependentUpdate() {
    const elmtId = ViewStackProcessor.GetElmtIdToAccountFor();
    if (elmtId < 0) {
      // not access recording 
      return;
    }
    console.debug(`ObservedPropertyAbstract[${this.id__()}, '${this.info() || "unknown"}']: recordDependentUpdate on elmtId ${elmtId}.`)
    this.dependentElementIds_.add(elmtId);
  }

  public markDependentElementsDirty(view: ViewPU) {
    if (!this.markDependentElementsIsPending) {
      return;
    }
    if (this.dependentElementIds_.size > 0) {
      console.debug(`ObservedPropertyAbstract[${this.id__()}, '${this.info() || "unknown"}']:markDependentElementsDirty`);
      this.dependentElementIds_.forEach(elmtId => {
        view.markElemenDirtyById(elmtId)
        console.debug(`   - elmtId ${elmtId}.`);
      });
    }
    this.markDependentElementsIsPending = false;
  }

  public purgeDependencyOnElmtId(rmElmtId: number): void {
    console.debug(`ObservedPropertyAbstract[${this.id__()}, '${this.info() || "unknown"}']:purgeDependencyOnElmtId ${rmElmtId}`);
    this.dependentElementIds_.delete(rmElmtId);
  }

  public SetPropertyUnchanged(): void {
    // function to be removed
    // keep it here until transpiler is updated.
  }

  // FIXME check, is this used from AppStorage.
  // unified Appstorage, what classes to use, and the API
  public createLink(subscribeOwner?: IPropertySubscriber,
    linkPropName?: PropertyInfo): ObservedPropertyAbstractPU<T> {
    throw new Error("Can not create a AppStorage 'Link' from a @State property. ");
  }

  public createProp(subscribeOwner?: IPropertySubscriber,
    linkPropName?: PropertyInfo): ObservedPropertyAbstractPU<T> {
    throw new Error("Can not create a AppStorage 'Prop' from a @State property. ");
  }
}