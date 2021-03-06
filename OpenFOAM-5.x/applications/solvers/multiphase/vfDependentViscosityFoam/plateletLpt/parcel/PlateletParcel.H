#ifndef PLATELETPARCEL_H
#define PLATELETPARCEL_H

#include "particle.H"

namespace Foam
{

template<class ParcelType>
class PlateletParcel;

// Forward declaration of friend functions
template<class ParcelType>
Ostream& operator<<
(
    Ostream&,
    const PlateletParcel<ParcelType>&
);

/*---------------------------------------------------------------------------*\
                         Class PlateletParcel Declaration
\*---------------------------------------------------------------------------*/

template<class ParcelType>
class PlateletParcel 
: 
	public ParcelType
{
public:
    using constantProperties = typename ParcelType::constantProperties;

	template<class CloudType>
    class TrackingData
    :
        public ParcelType::template TrackingData<CloudType>
    {
    private:

        // Private data

            // Interpolators for continuous phase fields

                //- Interpolator for continuous phase stress magnitude field
                autoPtr<interpolation<symmTensor> > tauInterp_;


    public:

        typedef typename ParcelType::template TrackingData<CloudType>::trackPart
            trackPart;

        // Constructors

            //- Construct from components
            inline TrackingData
            (
                CloudType& cloud,
                trackPart part = ParcelType::template
                    TrackingData<CloudType>::tpLinearTrack
            );

            inline const interpolation<symmTensor>& tauInterp() const { return tauInterp_(); }
    };


protected:

    // Protected data
        //- Accumulated mechanical dose
        scalar stressHistory_;

		//- Accumulated Platelet activation state
		scalar pas_;

        //- Shear rate history
		scalar stressRateHistory_;

		//- Shear stress magnitude
		symmTensor tau_;

		//- Stored value from last iteration
		symmTensor tau_last_;

public:

    // Static data members

        //- Runtime type information
        TypeName("PlateletParcel");

        //- String representation of properties
        AddToPropertyList
        (
            ParcelType,
			" stressHistory"
          + " pas"
          + " stressRateHistory"
		  + " (tauxx tauxy tauxz tauyy tauyz tauzz)"
        );

// Constructors

        //- Construct from owner, position, and cloud owner
        //  Other properties initialised as null
        inline PlateletParcel
        (
            const polyMesh& mesh,
            const vector& position,
            const label cellI,
            const label tetFaceI,
            const label tetPtI
        );

		//- Construct from a position and a cell, searching for the rest of the
        //  required topology. Other properties are initialised as null.
        inline PlateletParcel
        (
            const polyMesh& mesh,
            const vector& position,
            const label celli
        );

        //- Construct from components
        inline PlateletParcel
        (
            const polyMesh& mesh,
            const vector& position,
            const label cellI,
            const label tetFaceI,
            const label tetPtI,
            const label typeId,
            const scalar nParticle0,
            const scalar d0,
            const scalar dTarget0,
            const vector& U0,
            const vector& f0,
            const vector& angularMomentum0,
            const vector& torque0,
            const constantProperties& constProps
        );

        //- Construct from Istream
        PlateletParcel
        (
            const polyMesh& mesh,
            Istream& is,
            bool readFields = true
        );

        //- Construct as a copy
        PlateletParcel(const PlateletParcel& p);

        //- Construct as a copy
        PlateletParcel(const PlateletParcel& p, const polyMesh& mesh);

        //- Construct and return a (basic particle) clone
        virtual autoPtr<particle> clone() const
        {
            return autoPtr<particle>(new PlateletParcel(*this));
        }

        //- Construct and return a (basic particle) clone
        virtual autoPtr<particle> clone(const polyMesh& mesh) const
        {
            return autoPtr<particle>(new PlateletParcel(*this, mesh));
        }

        //- Factory class to read-construct particles used for
        //  parallel transfer
        class iNew
        {
            const polyMesh& mesh_;

        public:

            iNew(const polyMesh& mesh)
            :
                mesh_(mesh)
            {}

            autoPtr<PlateletParcel<ParcelType> > operator()(Istream& is) const
            {
                return autoPtr<PlateletParcel<ParcelType> >
                (
                    new PlateletParcel<ParcelType>(mesh_, is, true)
                );
            }
        };


		//- Access
			const scalar& pas() const { return pas_; }
            scalar& pas() { return pas_; }
            const scalar& stressHistory() const { return stressHistory_; }
            scalar& stressHistory() { return stressHistory_; }
            const scalar& stressRateHistory() const { return stressRateHistory_; }
            scalar& stressRateHistory() { return stressRateHistory_; }
			const symmTensor& tau() const { return tau_; }
			symmTensor& tau() { return tau_; }
            const symmTensor& tau_last() const { return tau_last_; }
			symmTensor& tau_last() { return tau_last_; }


		//- Main calculation loop

      		//- Set cell values
            template<class TrackData>
            void setCellValues
            (
                TrackData& td,
                const scalar dt,
                const label cellI
            );

			//- Update parcel properties over the time interval
            template<class TrackData>
            void calc
            (
                TrackData& td,
                const scalar dt,
                const label cellI
            );


       // I-O

            //- Read
            template<class CloudType>
            static void readFields(CloudType& c);

            //- Write
            template<class CloudType>
            static void writeFields(const CloudType& c);


    // Ostream Operator

        friend Ostream& operator<< <ParcelType>
        (
            Ostream&,
            const PlateletParcel<ParcelType>&
        );
};

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "PlateletParcelI.H"
#include "PlateletParcelIO.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#ifdef NoRepository
    #include "PlateletParcel.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
